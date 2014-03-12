#include <iostream>
#include <map>
#include <limits>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"
#include "q_utilities.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_server_kernels.hpp"
#include "q_gpu_srt_join.hpp"

using std::string;
using std::map;
using std::cerr;
using std::endl;
using std::numeric_limits;

using boost::shared_ptr;

namespace QDB {

  struct __srt_join_interface {
    __srt_join_interface () {
    }

    virtual ~__srt_join_interface () {
    }

    virtual int operator()(gpu_register const & RsrcLink,
                           gpu_register const & RsrcVal,
                           gpu_register const & RdstLink,
                           gpu_register & RdstVal,
                           gpu_register & RdstNn) {
      cerr << "ERROR: NOT IMPLEMENTED" << endl;
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T_LINK,
            typename T_VALUE,
            typename MERGE_OP>
  struct __srt_join : public __srt_join_interface {
    __srt_join(T_VALUE init = 0)
      : __srt_join_interface(),
        init_val(init),
        notfound_val(init),
        notfound_nn_val(0) {
    }

    __srt_join(T_VALUE init, T_VALUE notfound, Q_I1_t notfound_nn)
      : __srt_join_interface(),
        init_val(init),
        notfound_val(notfound),
        notfound_nn_val(notfound_nn) {
    }

    virtual ~__srt_join() {
    }

    virtual int operator()(gpu_register const & RsrcLink,
                           gpu_register const & RsrcVal,
                           gpu_register const & RdstLink,
                           gpu_register & RdstVal,
                           gpu_register & RdstNn) {
      dim3 dimGrid(NBLOCKS);
      dim3 dimBlock(NTHREADS);
      srt_join_kernel<T_LINK, T_VALUE, MERGE_OP><<< dimGrid, dimBlock >>>
        (static_cast<T_LINK const *>(RsrcLink.reg),
         static_cast<T_VALUE const *>(RsrcVal.reg),
         size_t(RsrcLink.nR),
         static_cast<T_LINK const *>(RdstLink.reg),
         static_cast<T_VALUE *>(RdstVal.reg),
         static_cast<Q_I1_t *>(RdstNn.reg),
         size_t(RdstLink.nR),
         MERGE_OP(),
         init_val,
         notfound_val,
         notfound_nn_val);
      return Q_PASS;
    }
    T_VALUE init_val;
    T_VALUE notfound_val;
    Q_I1_t notfound_nn_val;
  };

  struct srt_join {
    srt_join() {
      add_link_ops<I1>();
      add_link_ops<I2>();
      add_link_ops<I4>();
      add_link_ops<I8>();
    }

    int operator()(gpu_register const & RsrcLink,
                   gpu_register const & RsrcVal,
                   gpu_register const & RdstLink,
                   std::string const & str_op,
                   gpu_register & RdstVal,
                   gpu_register & RdstNn) {
      if ((op.find(str_op) == op.end()) ||
          (op[str_op].find(RsrcLink.fldtype) == op[str_op].end()) ||
          (op[str_op][RsrcLink.fldtype].find(RsrcVal.fldtype) == op[str_op][RsrcLink.fldtype].end())) {
        cerr << "ERROR: srt_join not implemented for "
             << "type(link)=" << get_fldtype_string(RsrcLink.fldtype) << " "
             << "type(value)=" << get_fldtype_string(RsrcVal.fldtype) << " "
             << "op=" << str_op << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }

      return op[str_op][RsrcLink.fldtype][RsrcVal.fldtype]->operator()(RsrcLink, RsrcVal, RdstLink,
                                                                       RdstVal, RdstNn);
    }

  private:
    template <typename T>
    inline
    void addop(string const & opname, FLD_TYPE link_type, FLD_TYPE val_type,
               T * oper) {
      op[opname][link_type][val_type] = shared_ptr<__srt_join_interface>
        (static_cast<__srt_join_interface *>(oper));
    }

    template <FLD_TYPE link, FLD_TYPE value>
    inline
    void addops() {
      typedef typename fldtype_traits<link>::q_type link_type;
      typedef typename fldtype_traits<value>::q_type value_type;
      typedef typename fldtype_traits<value>::unsigned_type unsigned_value_type;
      addop("reg", link, value, new __srt_join<link_type, value_type, SECOND<value_type> >);
      addop("min", link, value, new __srt_join<link_type, value_type, MIN<value_type> >(numeric_limits<value_type>::max()));
      addop("max", link, value, new __srt_join<link_type, value_type, MAX<value_type> >(numeric_limits<value_type>::min()));
      addop("sum", link, value, new __srt_join<link_type, value_type, ADD<value_type> >(0, 0, 1));
      addop("and", link, value, new __srt_join<link_type, value_type, BWISE_AND<value_type> >(value_type(~unsigned_value_type(0)), 0, 1));
      addop("or", link, value, new __srt_join<link_type, value_type, BWISE_OR<value_type> >(0, 0, 1));
      addop("cnt", link, value, new __srt_join<link_type, value_type, FIRST_PLUS_ONE<value_type> >(0, 0, 1));
    }

    template <FLD_TYPE link>
    inline
    void add_link_ops() {
      addops<link, I1>();
      addops<link, I2>();
      addops<link, I4>();
      addops<link, I8>();
    }

    typedef map<FLD_TYPE, shared_ptr<__srt_join_interface> > map_valuefld_to_oper;
    typedef map<FLD_TYPE,  map_valuefld_to_oper> map_linkfld_to_valuefld;
    typedef map<string,  map_linkfld_to_valuefld> map_opname_to_linkfld;

    map_opname_to_linkfld op;
  };

  static srt_join s_srt_join;

  int srt_join_GPU(gpu_register const & RsrcLink,
                   gpu_register const & RsrcVal,
                   gpu_register const & RdstLink,
                   std::string const & str_op,
                   gpu_register & RdstVal,
                   gpu_register & RdstNn) {
    return s_srt_join(RsrcLink, RsrcVal, RdstLink, str_op, RdstVal, RdstNn);
  }

}
