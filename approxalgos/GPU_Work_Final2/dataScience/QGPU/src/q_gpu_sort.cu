#include <thrust/device_ptr.h>
#include <thrust/sort.h>
#include <thrust/functional.h>
#include <string>
#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_utilities.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_sort.hpp"
#include "q_gpu_server_kernels.hpp"

using std::string;
using std::cerr;
using std::endl;
using std::map;

using boost::shared_ptr;

namespace QDB {

  struct __sort_interface {
    __sort_interface () {
    }

    virtual ~__sort_interface () {
    }

    virtual int operator()(gpu_register & R, string const & order) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __sort : public __sort_interface {
    __sort()
      : __sort_interface() {
    }

    virtual ~__sort() {
    }

    virtual int operator()(gpu_register & R, string const & order) {
      thrust::device_ptr<T> begin((T *)(R.reg));
      thrust::device_ptr<T> end(begin + R.nR);
      if (order == string("dsc")) {
        thrust::sort(begin, end, thrust::greater<T>());
      } else {
        thrust::sort(begin, end);
      }

      return Q_PASS;
    }
  };

  struct sort {
  public:
    sort() {
      addop<I1>();
      addop<I2>();
      addop<I4>();
      addop<I8>();
    }

    int operator()(gpu_register & R, string const & order) {
      if ((op.find(R.fldtype) == op.end()) ||
          ((order != string("asc")) && (order != string("dsc")))) {
        cerr << "ERROR: sort not supported for"
             << " type(fs)=" << get_fldtype_string(R.fldtype)
             << " order=" << order << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[R.fldtype]->operator()(R, order);
    }

  private:
    template <FLD_TYPE index>
    inline
    void addop() {
      op[index] = shared_ptr<__sort_interface>
        (static_cast<__sort_interface *>
         (new __sort< typename fldtype_traits<index>::q_type >));
    }
    map<FLD_TYPE, shared_ptr<__sort_interface> > op;
  };

  struct __sortf1f2_interface {
    __sortf1f2_interface () {
    }

    virtual ~__sortf1f2_interface () {
    }

    virtual int operator()(gpu_register & Rkey, gpu_register & Ridx, string const & srttype) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_KEY, typename T_IDX>
  struct __sortf1f2 : public __sortf1f2_interface {
    __sortf1f2()
      : __sortf1f2_interface() {
    }

    virtual ~__sortf1f2() {
    }

    virtual int operator()(gpu_register & Rkey, gpu_register & Ridx, string const & srttype) {
      if (Rkey.nR != Ridx.nR) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      thrust::device_ptr<T_KEY> keyBegin((T_KEY *)(Rkey.reg));
      thrust::device_ptr<T_KEY> keyEnd(keyBegin + Rkey.nR);
      thrust::device_ptr<T_IDX> idxBegin((T_IDX *)(Ridx.reg));
      if (srttype == string("A_")) {
        thrust::sort_by_key(keyBegin, keyEnd, idxBegin);
      } else if (srttype == string("D_")) {
        thrust::sort_by_key(keyBegin, keyEnd, idxBegin, thrust::greater<T_KEY>());
      } else {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return Q_PASS;
    }
  };

  struct sortf1f2 {
  public:
    sortf1f2() {
      addop<I1, I4>();
      addop<I2, I4>();
      addop<I4, I4>();
      addop<I8, I4>();
      addop<I1, I8>();
      addop<I2, I8>();
      addop<I4, I8>();
      addop<I8, I8>();
    }

    int operator()(gpu_register & Rkey, gpu_register & Rval, string const & srttype) {
      if ((op.find(Rkey.fldtype) == op.end()) ||
          (op[Rkey.fldtype].find(Rval.fldtype) == op[Rkey.fldtype].end()) ||
          ((srttype != string("A_")) && (srttype != string("D_")))) {
        cerr << "ERROR: sortf1f2 not supported for"
             << " type(key)=" << get_fldtype_string(Rkey.fldtype)
             << " type(val)=" << get_fldtype_string(Rval.fldtype)
             << " " << srttype << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[Rkey.fldtype][Rval.fldtype]->operator()(Rkey, Rval, srttype);
    }

  private:
    template <FLD_TYPE key, FLD_TYPE index>
    inline
    void addop() {
      typedef typename fldtype_traits<key>::q_type key_type;
      typedef typename fldtype_traits<index>::q_type index_type;
      op[key][index] = shared_ptr<__sortf1f2_interface>
        (static_cast<__sortf1f2_interface *>
         (new __sortf1f2< key_type, index_type > ));
    }
    typedef map<FLD_TYPE, shared_ptr<__sortf1f2_interface> > map_valfld_to_oper;
    typedef map<FLD_TYPE, map_valfld_to_oper> map_keyfld_to_valfld;
    map_keyfld_to_valfld op;
  };

  static sort s_sort;

  int
  sort_GPU(gpu_register & R, string const & srttype) {
    return s_sort(R, srttype);
  }

  static sortf1f2 s_sortf1f2;

  int
  sortf1f2_GPU(gpu_register & R1, gpu_register & R2, string const & srttype) {
    return s_sortf1f2(R1, R2, srttype);
  }

}
