#include <string>
#include <iostream>
#include <map>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_utilities.hpp"
#include "q_gpu_funnel.hpp"
#include "q_gpu_server_kernels.hpp"

using std::string;
using std::cerr;
using std::endl;
using std::map;

using std::ptr_fun;
using boost::shared_ptr;
using boost::lexical_cast;
using boost::bad_lexical_cast;

namespace QDB {

  struct __funnel_interface {
    __funnel_interface () {
    }

    virtual ~__funnel_interface () {
    }

    virtual int operator()(gpu_register const & Rkey,
                           string const & str_value,
                           gpu_register const & Rsame_session,
                           gpu_register const & Rprev_funnel,
                           gpu_register const & Rresult) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __funnel : public __funnel_interface {
    __funnel()
      : __funnel_interface() {
    }

    virtual ~__funnel() {
    }

    virtual int operator()(gpu_register const & Rkey,
                           string const & str_value,
                           gpu_register const & Rsame_session,
                           gpu_register const & Rprev_funnel,
                           gpu_register const & Rresult) {
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      T value;
      try {
        value = lexical_cast<T>(str_value);
      } catch (bad_lexical_cast const &) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      funnel_kernel<T>
        <<< dimGrid, dimBlock >>>
        ((T const *)(Rkey.reg), size_t(Rkey.nR),
         value,
         (Q_I1_t const *)(Rsame_session.reg),
         (Q_I1_t const *)(Rprev_funnel.reg),
         (Q_I1_t *)(Rresult.reg));
      return Q_PASS;
    }
  };

  struct funnel {
  public:
    funnel() {
      addop<I1>();
      addop<I2>();
      addop<I4>();
      addop<I8>();
    }

    virtual int operator()(gpu_register const & Rkey,
                           string const & str_value,
                           gpu_register const & Rsame_session,
                           gpu_register const & Rprev_funnel,
                           gpu_register const & Rresult) {
      if (op.find(Rkey.fldtype) == op.end()) {
        cerr << "ERROR: funnel not supported for "
             << "type(idx)=" << get_fldtype_string(Rkey.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[Rkey.fldtype]->operator()(Rkey, str_value, Rsame_session, Rprev_funnel, Rresult);
    }

  private:
    template <FLD_TYPE index>
    inline
    void addop() {
      typedef typename fldtype_traits<index>::q_type q_type;
      op[index] = shared_ptr<__funnel_interface>
        (static_cast<__funnel_interface *>(new __funnel<q_type>));
    }
    map<FLD_TYPE, shared_ptr<__funnel_interface> > op;
  };

  static funnel s_funnel;

  int
  funnel_GPU(gpu_register const & Rkey,
             string const & str_value,
             gpu_register const & Rsame_session,
             gpu_register const & Rprev_funnel,
             gpu_register & Rresult) {
    return s_funnel(Rkey, str_value, Rsame_session, Rprev_funnel, Rresult);
  }

  struct __funnel_count_interface {
    __funnel_count_interface () {
    }

    virtual ~__funnel_count_interface () {
    }

    virtual int operator()(gpu_register const & Rkey,
                           size_t distance,
                           gpu_register const & Rsame_session,
                           gpu_register const & Rprev_funnel_count,
                           gpu_register & Rresult) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_INDEX, typename T_RESULT>
  struct __funnel_count : public __funnel_count_interface {
    __funnel_count()
      : __funnel_count_interface() {
    }

    virtual ~__funnel_count() {
    }

    virtual int operator()(gpu_register const & Rkey,
                           size_t distance,
                           gpu_register const & Rsame_session,
                           gpu_register const & Rcurr_funnel,
                           gpu_register & Rresult) {
      if (cudaMemset(Rresult.reg, 0, Rresult.nR * sizeof(T_RESULT)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);

      size_t sharedMemSize = Rresult.nR * sizeof(T_RESULT);
      if (sharedMemSize > MAX_SHARED_MEM) {
        sharedMemSize = 0;
      }

      funnel_count_kernel<T_INDEX, T_RESULT>
        <<< dimGrid, dimBlock, sharedMemSize >>>
        ((T_INDEX const *)(Rkey.reg),
         (Q_I1_t const *)(Rsame_session.reg),
         (Q_I1_t const *)(Rcurr_funnel.reg),
         distance,
         (T_RESULT *)(Rresult.reg),
         size_t(Rkey.nR),
         size_t(Rresult.nR),
         sharedMemSize);
      return Q_PASS;
    }
  };

  struct funnel_count {
  public:
    funnel_count() {
      addops<I1>();
      addops<I2>();
      addops<I4>();
      addops<I8>();
    }

    int operator()(gpu_register const & Rkey,
                   size_t distance,
                   gpu_register const & Rsame_session,
                   gpu_register const & Rprev_funnel_count,
                   gpu_register & Rresult) {
      if ((op.find(Rkey.fldtype) == op.end()) ||
          (op[Rkey.fldtype].find(Rresult.fldtype) == op[Rkey.fldtype].end())) {
        cerr << "ERROR: funnel_count not supported for"
             << " type(key)=" << get_fldtype_string(Rkey.fldtype)
             << " type(result)=" << get_fldtype_string(Rresult.fldtype)
             << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[Rkey.fldtype][Rresult.fldtype]->operator()(Rkey, distance, Rsame_session, Rprev_funnel_count, Rresult);
    }

  private:
    typedef map<FLD_TYPE, shared_ptr<__funnel_count_interface> > map_resultfld_to_oper;
    typedef map<FLD_TYPE, map_resultfld_to_oper> map_keyfld_to_resultfld;

    template <FLD_TYPE key, FLD_TYPE result>
    inline
    void addop() {
      typedef typename fldtype_traits<key>::q_type key_type;
      typedef typename fldtype_traits<result>::q_type result_type;
      op[key][result] = shared_ptr<__funnel_count_interface>
        (static_cast<__funnel_count_interface *>(new __funnel_count<key_type, result_type>));
    }

    template <FLD_TYPE key>
    inline
    void addops() {
      //      addop<key, I1>();
      //      addop<key, I2>();
      addop<key, I4>();
      addop<key, I8>();
    }

    map_keyfld_to_resultfld op;
  };

  static funnel_count s_funnel_count;

  int
  funnel_count_GPU(gpu_register const & Rkey,
                   size_t distance,
                   gpu_register const & Rsame_session,
                   gpu_register const & Rcurr_funnel,
                   gpu_register & Rresult) {
    return s_funnel_count(Rkey, distance, Rsame_session, Rcurr_funnel, Rresult);
  }
};
