#include <thrust/logical.h>
#include <thrust/device_ptr.h>
#include <thrust/functional.h>
#include <thrust/execution_policy.h>
#include <string>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>

#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_server_kernels.hpp"
#include "q_gpu_count.hpp"

using std::string;
using std::cerr;
using std::endl;
using std::map;

using boost::shared_ptr;

namespace QDB {

  struct __are_all_indices_valid_interface {
    __are_all_indices_valid_interface() {
    }

    virtual ~__are_all_indices_valid_interface() {
    }

    virtual int operator()(gpu_register const & R, size_t ub, bool & result) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __are_all_indices_valid : public __are_all_indices_valid_interface {
    __are_all_indices_valid()
      : __are_all_indices_valid_interface() {
    }

    virtual ~__are_all_indices_valid() {
    }

    virtual int operator()(gpu_register const & R, size_t ub, bool & result) {
      int * d_val;
      int h_val = 0;
      if (make_d_copy(&d_val, h_val) != Q_PASS) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      is_any_op_kernel<T, INVALID_INDEX<T> >
        <<< dimGrid, dimBlock >>>
        ((T *)(R.reg), size_t(R.nR), INVALID_INDEX<T>(ub), d_val);
      if (restore_h_copy(h_val, &d_val) != Q_PASS) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      result = (h_val == 0);
      return Q_PASS;
    }
  };

  class are_all_indices_valid {
  public:
    are_all_indices_valid() {
      addop<I1>();
      addop<I2>();
      addop<I4>();
      addop<I8>();
    }

    int operator()(gpu_register const & R, size_t ub, bool & result) {
      if (op.find(R.fldtype) == op.end()) {
        cerr << "ERROR: field type " << get_fldtype_string(R.fldtype) << " not supported for count or countf" << endl;
        result = false;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return (op[R.fldtype]->operator()(R, ub, result) != Q_PASS);
    }

  private:

    template <FLD_TYPE index>
    inline
    void addop() {
      typedef typename fldtype_traits<index>::q_type q_type;
      op[index] = shared_ptr<__are_all_indices_valid_interface>
        (static_cast<__are_all_indices_valid_interface *>
         (new __are_all_indices_valid<q_type>));
    }
    map<FLD_TYPE, shared_ptr<__are_all_indices_valid_interface> > op;
  };

  static are_all_indices_valid s_are_all_indices_valid;

  struct __count_interface {
    __count_interface () {
    }

    virtual ~__count_interface () {
    }

    virtual int operator()(gpu_register const & Ridx,
                           gpu_register const * pRcond,
                           gpu_register & Rdst) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_INDEX, typename T_RESULT>
  struct __count : public __count_interface {
    __count()
      : __count_interface() {
    }

    virtual ~__count() {
    }

    virtual int operator()(gpu_register const & Ridx,
                           gpu_register const * pRcond,
                           gpu_register & Rdst) {
      if (cudaMemset(Rdst.reg, 0, Rdst.nR * sizeof(T_RESULT)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);

      size_t sharedMemSize = Rdst.nR * sizeof(T_RESULT);
      if (sharedMemSize > MAX_SHARED_MEM) {
        sharedMemSize = 0;
      }
      count_kernel<T_INDEX, Q_I1_t, T_RESULT>
        <<< dimGrid, dimBlock, sharedMemSize >>>
        ((T_INDEX *)(Ridx.reg),
         pRcond ? (Q_I1_t const *)(pRcond->reg) : NULL,
         NULL,
         (T_RESULT *)(Rdst.reg),
         Ridx.nR, Rdst.nR, sharedMemSize);

      return Q_PASS;
    }
  };

  struct count {
  public:
    count() {
      add_index_ops<I1>();
      add_index_ops<I2>();
      add_index_ops<I4>();
    }

    int operator()(gpu_register const & Ridx,
                   gpu_register const * pRcond,
                   gpu_register & Rdst,
                   bool bSafeMode) {
      if (bSafeMode) {
        bool allgood = true;
        if ((s_are_all_indices_valid(Ridx, Rdst.nR, allgood) != Q_PASS) ||
            !allgood) {
          cerr << "ERROR: index out of valid range [0.." << Rdst.nR << ")" << endl;
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
      }

      if ((op.find(Ridx.fldtype) == op.end()) ||
          (op[Ridx.fldtype].find(Rdst.fldtype) == op[Ridx.fldtype].end()) ||
          (pRcond && pRcond->fldtype != I1)) {
        cerr << "ERROR: count not supported for "
             << "type(fs)=" << get_fldtype_string(Ridx.fldtype) << " ";
        if (pRcond) {
          cerr << "type(fc)=" << get_fldtype_string(pRcond->fldtype) << " ";
        }
        cerr << "type(fd)=" << get_fldtype_string(Rdst.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[Ridx.fldtype][Rdst.fldtype]->operator()(Ridx, pRcond, Rdst);
    }

  private:

    template <FLD_TYPE index, FLD_TYPE result>
    inline
    void addop() {
      typedef typename fldtype_traits<index>::q_type index_type;
      typedef typename fldtype_traits<result>::q_type result_type;
      op[index][result] = shared_ptr<__count_interface>
        (static_cast<__count_interface *>
         (new __count<index_type, result_type>));
    }

    template <FLD_TYPE index>
    inline
    void add_index_ops() {
      addop<index, I4>();
      addop<index, I8>();
    }

    typedef map<FLD_TYPE, shared_ptr<__count_interface> > map_resultfld_to_oper;
    typedef map<FLD_TYPE, map_resultfld_to_oper> map_indexfld_to_resultfld;
    map_indexfld_to_resultfld op;
  };

  static count s_count;

  struct __countf_interface {
    __countf_interface() {
    }

    virtual ~__countf_interface() {
    }

    virtual int operator()(gpu_register const & Ridx,
                           gpu_register const * pRcond,
                           gpu_register const & Rval,
                           gpu_register & Rdst) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_INDEX, typename T_VALUE, typename T_RESULT>
  struct __countf : public __countf_interface {
    __countf()
      :__countf_interface() {
    }

    virtual ~__countf() {
    }

    virtual int operator()(gpu_register const & Ridx,
                           gpu_register const * pRcond,
                           gpu_register const & Rval,
                           gpu_register & Rdst) {
      if (cudaMemset(Rdst.reg, 0, Rdst.nR * sizeof(T_RESULT)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      size_t sharedMemSize = Rdst.nR * sizeof(T_RESULT);
      if (sharedMemSize > MAX_SHARED_MEM) {
        sharedMemSize = 0;
      }
      count_kernel<T_INDEX, T_VALUE, T_RESULT>
        <<< dimGrid, dimBlock, sharedMemSize >>>
        ((T_INDEX const *)(Ridx.reg),
         pRcond ? (Q_I1_t const *)(pRcond->reg) : NULL,
         (T_VALUE const *)(Rval.reg),
         (T_RESULT *)(Rdst.reg),
         Ridx.nR, Rdst.nR, sharedMemSize);
      return Q_PASS;
    }
  };

  struct countf {
    countf() {
      add_index_ops<I1>();
      add_index_ops<I2>();
      add_index_ops<I4>();
    }

    int operator()(gpu_register const & Ridx,
                   gpu_register const * pRcond,
                   gpu_register const & Rval,
                   gpu_register & Rdst,
                   bool bSafeMode) {
      if (bSafeMode) {
        bool allgood = true;
        if ((s_are_all_indices_valid(Ridx, Rdst.nR, allgood) != Q_PASS) ||
            !allgood) {
          cerr << "ERROR: index out of valid range [0.." << Rdst.nR << ")" << endl;
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
      }

      if ((op.find(Ridx.fldtype) == op.end()) ||
          (op[Ridx.fldtype].find(Rval.fldtype) == op[Ridx.fldtype].end()) ||
          (op[Ridx.fldtype][Rval.fldtype].find(Rdst.fldtype) == op[Ridx.fldtype][Rval.fldtype].end()) ||
          (pRcond && (pRcond->fldtype != I1))) {
        cerr << "ERROR: countf not supported for "
             << "type(fs)=" << get_fldtype_string(Ridx.fldtype) << " "
             << "type(fv)=" << get_fldtype_string(Rval.fldtype) << " ";
        if (pRcond) {
          cerr << "type(fc)=" << get_fldtype_string(pRcond->fldtype) << " ";
        }
        cerr << "type(fd)=" << get_fldtype_string(Rdst.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[Ridx.fldtype][Rval.fldtype][Rdst.fldtype]->operator()(Ridx, pRcond, Rval, Rdst);
    }

  private:
    template <FLD_TYPE index, FLD_TYPE value, FLD_TYPE result>
    inline
    void addop() {
      typedef typename fldtype_traits<index>::q_type index_type;
      typedef typename fldtype_traits<value>::q_type value_type;
      typedef typename fldtype_traits<result>::q_type result_type;
      op[index][value][result] =
        shared_ptr<__countf_interface>
        (static_cast<__countf_interface *>
         (new __countf<index_type, value_type, result_type>));
    }

    template <FLD_TYPE index>
    inline
    void add_index_ops() {
      addop<index, I1, I4>();
      addop<index, I2, I4>();
      addop<index, I4, I4>();
      // addop<index, I8, I4>(); not supported since we do not want to sum I8s into an I4 result
      addop<index, I1, I8>();
      addop<index, I2, I8>();
      addop<index, I4, I8>();
      addop<index, I8, I8>();

      addop<index, F4, F4>();
      addop<index, F8, F8>();
    }

    typedef map<FLD_TYPE, shared_ptr<__countf_interface> > map_resultfld_to_oper;
    typedef map<FLD_TYPE, map_resultfld_to_oper> map_valuefld_to_resultfld;
    typedef map<FLD_TYPE, map_valuefld_to_resultfld> map_indexfld_to_valuefld;
    map_indexfld_to_valuefld op;
  };

  static countf s_countf;

  int
  count_GPU(gpu_register const & Ridx,
            gpu_register const * pRcond,
            gpu_register & Rdst,
            bool bSafeMode) {
    return s_count(Ridx, pRcond, Rdst, bSafeMode);
  }

  int
  countf_GPU(gpu_register const & Ridx,
             gpu_register const * pRcond,
             gpu_register const & Rval,
             gpu_register & Rdst,
             bool bSafeMode) {
    return s_countf(Ridx, pRcond, Rval, Rdst, bSafeMode);
  }

}
