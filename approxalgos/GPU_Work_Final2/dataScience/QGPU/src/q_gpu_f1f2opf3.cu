#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_utilities.hpp"
#include "q_gpu_server_kernels.hpp"
#include "q_gpu_f1f2opf3.hpp"

using std::string;
using std::map;
using std::cerr;
using std::endl;

using boost::shared_ptr;

namespace QDB {
  struct __f1f2opf3_interface {
    __f1f2opf3_interface() {
    }

    virtual ~__f1f2opf3_interface() {
    }

    virtual int operator()(gpu_register const & R1in,
                           gpu_register const & R2in,
                           gpu_register & Rout) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_IN,
            typename T_OUT,
            typename OP_TYPE>
  struct __f1f2opf3 : public __f1f2opf3_interface {
    __f1f2opf3()
      : __f1f2opf3_interface() {
    }

    virtual ~__f1f2opf3() {
    }

    virtual int operator()(gpu_register const & R1in,
                           gpu_register const & R2in,
                           gpu_register & Rout) {
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      f1f2opf3_kernel<T_IN, T_OUT><<< dimGrid, dimBlock >>>
        (static_cast<T_IN const *>(R1in.reg),
         static_cast<T_IN const *>(R2in.reg),
         static_cast<T_OUT *>(Rout.reg),
         size_t(R1in.nR), OP_TYPE());
      return Q_PASS;
    }
  };

  struct f1f2opf3 {
    f1f2opf3() {
      add_integer_ops<I1>();
      add_integer_ops<I2>();
      add_integer_ops<I4>();
      add_integer_ops<I8>();

      add_real_ops<F4>();
      add_real_ops<F8>();

      addop<I1, I2, CONCAT_FT<I1, I2> >("concat");
      addop<I2, I4, CONCAT_FT<I2, I4> >("concat");
      addop<I4, I8, CONCAT_FT<I4, I8> >("concat");
    }

    int operator()(gpu_register const & R1in,
                   gpu_register const & R2in,
                   string const & str_op,
                   gpu_register & Rout) {
      if ((op.find(str_op) == op.end()) ||
          (op[str_op].find(R1in.fldtype) == op[str_op].end()) ||
          (op[str_op][R1in.fldtype].find(Rout.fldtype) == op[str_op][R1in.fldtype].end())) {
        cerr << "ERROR: f1f2opf3 not implemented for "
             << "type(f1)=" << get_fldtype_string(R1in.fldtype) << " "
             << "type(f2)=" << get_fldtype_string(R2in.fldtype) << " "
             << "op=" << str_op << " "
             << "type(f3)=" << get_fldtype_string(Rout.fldtype) << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[str_op][R1in.fldtype][Rout.fldtype]->operator()(R1in, R2in, Rout);
    }

  private:
    template <FLD_TYPE f1type, FLD_TYPE f3type, typename OP>
    inline
    void addop(string const & opname) {
      op[opname][f1type][f3type] =
        shared_ptr<__f1f2opf3_interface>
        (static_cast<__f1f2opf3_interface *>
         (new __f1f2opf3<
          typename fldtype_traits<f1type>::q_type,
          typename fldtype_traits<f3type>::q_type,
          OP >));
    }

    template <FLD_TYPE ftype>
    inline
    void add_integer_ops() {
      addop<ftype, ftype, ADD_FT<ftype> >("+");
      addop<ftype, ftype, SUB_FT<ftype> >("-");
      addop<ftype, ftype, MUL_FT<ftype> >("*");
      addop<ftype, ftype, DIV_FT<ftype> >("/");
      addop<ftype, ftype, REM_FT<ftype> >("%");
      addop<ftype, I1, AND_FT<ftype> >("&&");
      addop<ftype, I1, OR_FT<ftype> >("||");
      addop<ftype, I1, ANDNOT_FT<ftype> >("&&!");
      addop<ftype, I1, CMP_GT_FT<ftype> >(">");
      addop<ftype, I1, CMP_LT_FT<ftype> >("<");
      addop<ftype, I1, CMP_GE_FT<ftype> >(">=");
      addop<ftype, I1, CMP_LE_FT<ftype> >("<=");
      addop<ftype, I1, CMP_EQ_FT<ftype> >("==");
      addop<ftype, I1, CMP_NE_FT<ftype> >("!=");
      addop<ftype, ftype, BWISE_AND_FT<ftype> >("&");
      addop<ftype, ftype, BWISE_OR_FT<ftype> >("|");
      addop<ftype, ftype, BWISE_XOR_FT<ftype> >("^");
      addop<ftype, ftype, SHIFT_LEFT_FT<ftype> >("<<");
      addop<ftype, ftype, SHIFT_RIGHT_FT<ftype> >(">>");
    }

    template <FLD_TYPE ftype>
    inline
    void add_real_ops() {
      addop<ftype, ftype, ADD_FT<ftype> >("+");
      addop<ftype, ftype, SUB_FT<ftype> >("-");
      addop<ftype, ftype, MUL_FT<ftype> >("*");
      addop<ftype, ftype, DIV_FT<ftype> >("/");
      addop<ftype, I1, CMP_GT_FT<ftype> >(">");
      addop<ftype, I1, CMP_LT_FT<ftype> >("<");
      addop<ftype, I1, CMP_GE_FT<ftype> >(">=");
      addop<ftype, I1, CMP_LE_FT<ftype> >("<=");
      addop<ftype, I1, CMP_EQ_FT<ftype> >("==");
      addop<ftype, I1, CMP_NE_FT<ftype> >("!=");
    }

    typedef map<FLD_TYPE, shared_ptr<__f1f2opf3_interface> > map_f3_to_oper;
    typedef map<FLD_TYPE,  map_f3_to_oper> map_f1f2_to_f3;
    typedef map<string,  map_f1f2_to_f3> map_opname_to_f1f2;
    map_opname_to_f1f2 op;
  };

  static f1f2opf3 s_f1f2opf3;

  int f1f2opf3_GPU(gpu_register const & R1in,
                   gpu_register const & R2in,
                   string const & str_op,
                   gpu_register & Rout) {
    return s_f1f2opf3(R1in, R2in, str_op, Rout);
  }

}
