#include <limits>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_utilities.hpp"
#include "q_gpu_server_kernels.hpp"
#include "q_gpu_f_to_s.hpp"

using std::string;
using std::map;
using std::cerr;
using std::endl;
using std::numeric_limits;
using std::ostringstream;

using boost::shared_ptr;

#include <sys/time.h>

namespace QDB {

  class GPU_timer {
  public:
    GPU_timer(string const & nm)
      : name(nm),
        start(),
        end() {
      gettimeofday(&start, NULL);
    }
    ~GPU_timer() {
      gettimeofday(&end, NULL);
      double diff = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
      std::cout << "Time Taken for " << name << ": " << diff << " usec" << std::endl;
    }

  private:
    string name;
    struct timeval start, end;
  };

  struct __f_to_s_interface {
    __f_to_s_interface() {
    }

    virtual ~__f_to_s_interface() {
    }

    virtual int operator()(gpu_register const & R,
                           gpu_register const * pRfilter1,
                           gpu_register const * pRfilter2,
                           string & str_result) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T,
            typename T_RESULT,
            typename OP_TYPE>
  struct __f_to_s : public __f_to_s_interface {
    __f_to_s(T_RESULT init_val = 0)
      : __f_to_s_interface(),
        m_initVal(init_val) {
    }

    virtual ~__f_to_s() {
    }

    virtual int operator()(gpu_register const & R,
                           gpu_register const * pRfilter1,
                           gpu_register const * pRfilter2,
                           string & str_result) {
      T_RESULT * pPartial;
      if (cudaMalloc((void **)(&pPartial), NBLOCKS * sizeof(T_RESULT)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      {
        dim3 dimGrid(NBLOCKS);
        dim3 dimBlock(NTHREADS);
        size_t sharedMemSize = dimBlock.x * sizeof(T_RESULT);
        if (sharedMemSize > MAX_SHARED_MEM) {
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
        filter_reduce_to_partial_kernel<T, T_RESULT, OP_TYPE>
          <<< dimGrid, dimBlock, sharedMemSize >>>
          (static_cast<T const *>(R.reg),
           size_t(R.nR),
           pPartial,
           static_cast<Q_I1_t const *>(pRfilter1 ? pRfilter1->reg : NULL),
           static_cast<Q_I1_t const *>(pRfilter2 ? pRfilter2->reg : NULL),
           OP_TYPE(),
           m_initVal);
      }
      {
        dim3 dimGrid(1);
        dim3 dimBlock(NTHREADS);
        reduce_partial_results_kernel<T_RESULT, OP_TYPE>
          <<< dimGrid, dimBlock >>>
          (pPartial,
           NBLOCKS,
           OP_TYPE(),
           m_initVal);
        T_RESULT result;
        if (cudaMemcpy((void *)(&result), (void const *)(pPartial), sizeof(T_RESULT), cudaMemcpyDeviceToHost) != cudaSuccess) {
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
        str_result = static_cast<ostringstream*>( &(ostringstream() << result) )->str();
      }
      if (cudaFree(pPartial) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return Q_PASS;
    }
    T_RESULT const m_initVal;
  };

  // specialization for summation

  template <typename T>
  struct __f_to_s<T, Q_I8_t, ADD<Q_I8_t> > : public __f_to_s_interface {
    __f_to_s()
      : __f_to_s_interface() {
    }

    virtual ~__f_to_s() {
    }

    virtual int operator()(gpu_register const & R,
                           gpu_register const * pRfilter1,
                           gpu_register const * pRfilter2,
                           string & str_result) {
      unsigned long long int * d_sum;
      if (cudaMalloc((void **)(&d_sum), sizeof(unsigned long long int)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      if (cudaMemset(d_sum, 0, sizeof(unsigned long long int)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      size_t sharedMemSize = dimBlock.x * sizeof(Q_I8_t);
      if (sharedMemSize > MAX_SHARED_MEM) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      filter_sum_kernel<T><<< dimGrid, dimBlock, sharedMemSize >>>
        ((T *)(R.reg), size_t(R.nR),
         (Q_I1_t const *)(pRfilter1 ? pRfilter1->reg : NULL),
         (Q_I1_t const *)(pRfilter2 ? pRfilter2->reg : NULL),
         d_sum);

      unsigned long long int tmp_result;
      if (cudaMemcpy((void *)(&tmp_result), (void const *)(d_sum), sizeof(unsigned long long int), cudaMemcpyDeviceToHost) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      Q_I8_t result = Q_I8_t(tmp_result);
      str_result = static_cast<ostringstream*>( &(ostringstream() << result) )->str();
      if (cudaFree(d_sum) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return Q_PASS;
    }
  };

  struct f_to_s {
  public:
    f_to_s() {
      add_integer_ops<I1>();
      add_integer_ops<I2>();
      add_integer_ops<I4>();
      add_integer_ops<I8>();
    }

    int operator()(gpu_register const & R,
                   gpu_register const * pRnn,
                   string const & str_op,
                   string & result) {
      if ((op.find(str_op) == op.end()) ||
          (op[str_op].find(R.fldtype) == op[str_op].end())) {
        cerr << "ERROR: f_to_s and f1f2_to_s not implemented for "
             << "type(f)=" << get_fldtype_string(R.fldtype) << " "
             << "op=" << str_op << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[str_op][R.fldtype]->operator()(R, pRnn, NULL, result);
    }

  private:

    template <typename T>
    inline
    void addop(string const & opname, FLD_TYPE ftype, T * oper) {
      op[opname][ftype] = shared_ptr<__f_to_s_interface>(static_cast<__f_to_s_interface *>(oper));
    }

    template <FLD_TYPE ftype>
    inline
    void add_integer_ops() {
      typedef typename fldtype_traits<ftype>::q_type q_type;
      addop("min", ftype, new __f_to_s<q_type, q_type, MIN<q_type> >(numeric_limits<q_type>::max()));
      addop("max", ftype, new __f_to_s<q_type, q_type, MAX<q_type> >(numeric_limits<q_type>::min()));
      addop("sum", ftype, new __f_to_s<q_type, Q_I8_t, ADD<Q_I8_t> >());
    }

    typedef map<FLD_TYPE, shared_ptr<__f_to_s_interface> > map_ftype_to_oper;
    typedef map<string, map_ftype_to_oper> map_opname_to_ftype;
    map_opname_to_ftype op;
  };

  static f_to_s s_f_to_s;

  int f_to_s_GPU(gpu_register const & R,
                      gpu_register const * pRnn,
                      string const & str_op,
                      string & result) {
    return s_f_to_s(R, pRnn, str_op, result);
  }
}
