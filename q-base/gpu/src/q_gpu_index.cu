#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/sequence.h>
#include <string>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_utilities.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_index.hpp"
#include "q_gpu_server_kernels.hpp"

using std::string;
using std::cerr;
using std::endl;
using std::map;

using boost::shared_ptr;

namespace QDB {

  struct __mk_idx_interface {
    __mk_idx_interface () {
    }

    virtual ~__mk_idx_interface () {
    }

    virtual int operator()(gpu_register & Ridx) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __mk_idx : public __mk_idx_interface {
    __mk_idx()
      : __mk_idx_interface() {
    }

    virtual ~__mk_idx() {
    }

    virtual int operator()(gpu_register & Ridx) {
      thrust::sequence(thrust::device_ptr<T>((T *)(Ridx.reg)),
                       thrust::device_ptr<T>((T *)(Ridx.reg) + size_t(Ridx.nR)));
      return Q_PASS;
    }
  };

  struct mk_idx {
  public:
    mk_idx() {
      addop<I1>();
      addop<I2>();
      addop<I4>();
      addop<I8>();
    }

    int operator()(gpu_register & Ridx) {
      if (op.find(Ridx.fldtype) == op.end()) {
        cerr << "ERROR: mk_idx not supported for "
             << "type(idx)=" << get_fldtype_string(Ridx.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[Ridx.fldtype]->operator()(Ridx);
    }

  private:
    template <FLD_TYPE index>
    inline
    void addop() {
      typedef typename fldtype_traits<index>::q_type q_type;
      op[index] = shared_ptr<__mk_idx_interface>
        (static_cast<__mk_idx_interface *>(new __mk_idx<q_type>));
    }
    map<FLD_TYPE, shared_ptr<__mk_idx_interface> > op;
  };

  struct __permute_by_idx_interface {
    __permute_by_idx_interface () {
    }

    virtual ~__permute_by_idx_interface () {
    }

    virtual int operator()(gpu_register const & Rdata,
                           gpu_register const & Ridx,
                           bool bForward,
                           gpu_register & Rresult) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_DATA, typename T_IDX>
  struct __permute_by_idx : public __permute_by_idx_interface {
    __permute_by_idx()
      : __permute_by_idx_interface() {
    }

    virtual ~__permute_by_idx() {
    }

    virtual int operator()(gpu_register const & Rdata,
                           gpu_register const & Ridx,
                           bool bForward,
                           gpu_register & Rresult) {
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      permute_by_idx_kernel<T_DATA, T_IDX> <<< dimGrid, dimBlock >>>
        ((T_DATA const *)(Rdata.reg),
         (T_IDX const *)(Ridx.reg),
         size_t(Rdata.nR),
         bForward,
         (T_DATA *)(Rresult.reg));
      return Q_PASS;
    }
  };

  struct permute_by_idx {
  public:
    permute_by_idx() {
      addops<I1>();
      addops<I2>();
      addops<I4>();
      addops<I8>();
      addops<F4>();
      addops<F8>();
    }

    int operator()(gpu_register const & Rdata,
                   gpu_register const & Ridx,
                   bool bForward,
                   gpu_register & Rresult) {
      if ((op.find(Rdata.fldtype) == op.end()) ||
          op[Rdata.fldtype].find(Ridx.fldtype) == op[Rdata.fldtype].end()) {
        cerr << "ERROR: permute_by_idx not supported for "
             << "type(data)=" << get_fldtype_string(Rdata.fldtype)
             << "type(idx)=" << get_fldtype_string(Ridx.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[Rdata.fldtype][Ridx.fldtype]->operator()(Rdata, Ridx, bForward, Rresult);
    }

  private:
    template <FLD_TYPE data, FLD_TYPE idx>
    inline
    void addop() {
      typedef typename fldtype_traits<data>::q_type q_datatype;
      typedef typename fldtype_traits<idx>::q_type q_idxtype;
      op[data][idx] = shared_ptr<__permute_by_idx_interface>
        (static_cast<__permute_by_idx_interface *>
         (new __permute_by_idx<q_datatype, q_idxtype>));
    }

    template <FLD_TYPE data>
    inline
    void addops() {
      addop<data, I1>();
      addop<data, I2>();
      addop<data, I4>();
      addop<data, I8>();
    }

    typedef map<FLD_TYPE, shared_ptr<__permute_by_idx_interface> > map_idxfld_to_oper;
    typedef map<FLD_TYPE, map_idxfld_to_oper> map_datafld_to_idxfld;
    map_datafld_to_idxfld op;
  };

  static mk_idx s_mk_idx;

  int
  mk_idx_GPU(gpu_register & Ridx) {
    return s_mk_idx(Ridx);
  }

  static permute_by_idx s_permute_by_idx;

  int
  permute_by_idx_GPU(gpu_register const & Rdata,
                     gpu_register const & Ridx,
                     bool bForward,
                     gpu_register & Rresult) {
    return s_permute_by_idx(Rdata, Ridx, bForward, Rresult);
  }
}
