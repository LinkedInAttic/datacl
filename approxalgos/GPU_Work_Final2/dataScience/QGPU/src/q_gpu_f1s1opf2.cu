#include <iostream>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_utilities.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_server_kernels.hpp"
#include "q_gpu_f1s1opf2.hpp"

using std::string;
using std::map;
using std::cerr;
using std::endl;

using boost::shared_ptr;

namespace QDB {
  struct __f1s1opf2_interface {
    __f1s1opf2_interface() {
    }

    virtual ~__f1s1opf2_interface() {
    }

    virtual int operator()(gpu_register const & Rin,
                           string const & str_scalar,
                           gpu_register & Rout) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };


  template <typename T_IN,
            typename T_OUT,
            typename OP_TYPE>
  struct __f1s1opf2 : public __f1s1opf2_interface {
    __f1s1opf2()
      : __f1s1opf2_interface() {
    }

    virtual ~__f1s1opf2() {
    }

    virtual int operator()(gpu_register const & Rin,
                           string const & str_scalar,
                           gpu_register & Rout) {
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      T_IN scalar = T_IN(strtoll(str_scalar.c_str(), NULL, 10));
      f1s1opf2_kernel<T_IN, T_OUT><<< dimGrid, dimBlock >>>
        (static_cast<T_IN const *>(Rin.reg),
         scalar,
         static_cast<T_OUT *>(Rout.reg),
         size_t(Rin.nR),
         OP_TYPE());
      return Q_PASS;
    }
  };

  struct f1s1opf2 {
    f1s1opf2() {
      add_integer_ops<I1>();
      add_integer_ops<I2>();
      add_integer_ops<I4>();
      add_integer_ops<I8>();

      add_real_ops<F4>();
      add_real_ops<F8>();
    }

    int operator()(gpu_register const & Rin,
                   string const & str_scalar,
                   string const & str_op,
                   gpu_register & Rout) {
      if ((op.find(str_op) == op.end()) ||
          (op[str_op].find(Rin.fldtype) == op[str_op].end()) ||
          (op[str_op][Rin.fldtype].find(Rout.fldtype) == op[str_op][Rin.fldtype].end())) {
        cerr << "ERROR: f1s1opf2 not implemented for"
             << " type(f1,s1)=" << get_fldtype_string(Rin.fldtype)
             << " op=" << str_op
             << " type(f2)=" << get_fldtype_string(Rout.fldtype) << endl;

        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return op[str_op][Rin.fldtype][Rout.fldtype]->operator()(Rin, str_scalar, Rout);
    }

  private:
    template <FLD_TYPE f1s1type, FLD_TYPE f2type, typename OP>
    inline
    void addop(string const & opname) {
      op[opname][f1s1type][f2type] =
        shared_ptr<__f1s1opf2_interface>
        (static_cast<__f1s1opf2_interface *>
         (new __f1s1opf2<
          typename fldtype_traits<f1s1type>::q_type,
          typename fldtype_traits<f2type>::q_type,
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
      addop<ftype, I1, CMP_GT_FT<ftype> >(">");
      addop<ftype, I1, CMP_LT_FT<ftype> >("<");
      addop<ftype, I1, CMP_GE_FT<ftype> >(">=");
      addop<ftype, I1, CMP_LE_FT<ftype> >("<=");
      addop<ftype, I1, CMP_EQ_FT<ftype> >("==");
      addop<ftype, I1, CMP_NE_FT<ftype> >("!=");
      addop<ftype, ftype, BWISE_AND_FT<ftype> >("&");
      addop<ftype, ftype, BWISE_OR_FT<ftype> >("|");
      addop<ftype, ftype, BWISE_XOR_FT<ftype> >("^");
      addop<ftype, I1, SHIFT_LEFT_FT<ftype> >("<<");
      addop<ftype, I1, SHIFT_RIGHT_FT<ftype> >(">>");
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

    typedef map<FLD_TYPE, shared_ptr<__f1s1opf2_interface> > map_f2_to_oper;
    typedef map<FLD_TYPE, map_f2_to_oper> map_f1_to_f2;
    typedef map<string, map_f1_to_f2> map_opname_to_f1;
    map_opname_to_f1 op;
  };

  static f1s1opf2 s_f1s1opf2;

  int f1s1opf2_GPU(gpu_register const & Rin,
                   string const & str_scalar,
                   string const & str_op,
                   gpu_register & Rout) {
    return s_f1s1opf2(Rin, str_scalar, str_op, Rout);
  }
}
