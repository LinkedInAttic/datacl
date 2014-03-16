#include "q_gpu_f1_shift_f2.hpp"
#include <thrust/fill.h>
#include <thrust/device_ptr.h>
#include <iostream>
#include <map>
#include <limits>
#include <memory>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_utilities.hpp"
#include "q_gpu_server_kernels.hpp"

using std::numeric_limits;
using std::cerr;
using std::endl;
using std::map;

using boost::shared_ptr;

namespace QDB {

  template <typename T>
  static inline int
  fill_device_memory(T * start, size_t sz, T value) {
    if (value) {
      boost::shared_ptr<T> h_start((T *)(malloc(sz * sizeof(T))), std::ptr_fun(free));
      if (!h_start.get()) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      for (T * i = h_start.get(), * iEnd = h_start.get() + sz; i != iEnd; ++i) {
        *i = value;
      }
      if (cudaMemcpy((char *)start, (char const *)(h_start.get()),
                     sz * sizeof(T),cudaMemcpyHostToDevice) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      /*
      // TODO: figure out why the thrust version below is slower than the above.
      thrust::fill(thrust::device_ptr<T>(start),
      thrust::device_ptr<T>(start + sz),
      value);
      */
    } else {
      if (cudaMemset((char *)start, 0, sz * sizeof(T)) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    }
    return Q_PASS;
  }

  template <typename T>
  static inline int
  copy_device_memory(T * dst, T const * src, size_t sz) {
    return ((cudaMemcpy((char *)dst, (char const *)src, sz * sizeof(T), cudaMemcpyDeviceToDevice) != cudaSuccess) ? Q_FAIL : Q_PASS);
  }

  struct __f1_shift_f2_interface {
    __f1_shift_f2_interface() {
    }

    virtual ~__f1_shift_f2_interface() {
    }

    virtual int operator()(gpu_register const & Rin,
                           gpu_register & Rout,
                           int n_shift,
                           Q_I8_t fill_value) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __f1_shift_f2 : public __f1_shift_f2_interface {
    __f1_shift_f2()
      : __f1_shift_f2_interface() {
    }

    virtual ~__f1_shift_f2() {
    }

    virtual int operator()(gpu_register const & Rin,
                           gpu_register & Rout,
                           int n_shift,
                           Q_I8_t fill_value) {
      if (fill_value < Q_I8_t(numeric_limits<T>::min()) ||
          fill_value > Q_I8_t(numeric_limits<T>::max())) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      T value = T(fill_value);
      if (n_shift == 0) {
        if (copy_device_memory<T>((T *)(Rout.reg), (T const *)(Rin.reg), Rin.nR) != Q_PASS) {
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
      } else if ((abs(n_shift) >= Rin.nR)) {
        if (fill_device_memory<T>((T *)(Rout.reg), Rout.nR, value) != Q_PASS) {
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
      } else {
        if (n_shift > 0) {
          if (fill_device_memory<T>((T *)(Rout.reg), n_shift, value) != Q_PASS) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
          if (copy_device_memory<T>((T *)(Rout.reg) + n_shift, (T const *)(Rin.reg), (Rin.nR - n_shift)) != Q_PASS) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
        } else {
          if (fill_device_memory<T>((T *)(Rout.reg) + (Rin.nR + n_shift), size_t(-n_shift), value) != Q_PASS) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
          if (copy_device_memory<T>((T *)(Rout.reg), (T const *)(Rin.reg) - n_shift, (Rin.nR + n_shift)) != Q_PASS) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
        }
      }
      return Q_PASS;
    }
  };

  class f1_shift_f2 {
  public:
    f1_shift_f2() {
      addop(I1, new __f1_shift_f2<Q_I1_t>);
      addop(I2, new __f1_shift_f2<Q_I2_t>);
      addop(I4, new __f1_shift_f2<Q_I4_t>);
      addop(I8, new __f1_shift_f2<Q_I8_t>);
    }

    int operator()(gpu_register const & Rin,
                   gpu_register & Rout,
                   int n_shift,
                   Q_I8_t fill_value) {
      if (op.find(Rin.fldtype) == op.end()) {
        cerr << "f1_shift_f2 not implemented for "
             << get_fldtype_string(Rin.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[Rin.fldtype]->operator()(Rin, Rout, n_shift, fill_value);
    }

  private:
    template <typename T>
    inline
    void addop(FLD_TYPE index, T * oper) {
      op[index] = shared_ptr<__f1_shift_f2_interface>
        (static_cast<__f1_shift_f2_interface *>(oper));
    }
    map<FLD_TYPE, shared_ptr<__f1_shift_f2_interface> > op;
  };

  static f1_shift_f2 s_f1_shift_f2;

  int f1_shift_f2_GPU(gpu_register const & Rin,
                      gpu_register & Rout,
                      int n_shift,
                      Q_I8_t fill_value) {
    return s_f1_shift_f2(Rin, Rout, n_shift, fill_value);
  }

}
