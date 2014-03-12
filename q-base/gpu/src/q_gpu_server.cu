#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <iostream>
#include <iterator>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <functional>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "q_gpu_server.hpp"
#include "q_gpu.hpp"
#include "q_gpu_server_kernels.hpp"

#include "q_utilities.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_f1s1opf2.hpp"
#include "q_gpu_f1f2opf3.hpp"
#include "q_gpu_f_to_s.hpp"
#include "q_gpu_count.hpp"
#include "q_gpu_f1_shift_f2.hpp"
#include "q_gpu_srt_join.hpp"
#include "q_gpu_sort.hpp"
#include "q_gpu_index.hpp"
#include "q_gpu_funnel.hpp"

using namespace std;


namespace QDB {

  //==================================================================
  //Operations on GPU
  //==================================================================

  int
  f1s1opf2_GPU(
               string const & d_fld1,
               string const & str_scalar,
               string const & str_op,
               string const & d_fld2
               ) {
    if (!GPU::getInstance().is_valid_register(d_fld1) ||
        !GPU::getInstance().is_valid_register(d_fld2)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return f1s1opf2_GPU(GPU::getInstance()[d_fld1], str_scalar, str_op, GPU::getInstance()[d_fld2]);
  }

  int
  f1f2opf3_GPU(
               string const & d_fld1,
               string const & d_fld2,
               string const & str_op,
               string const & d_fld3
               ) {
    if (!GPU::getInstance().is_valid_register(d_fld1) ||
        !GPU::getInstance().is_valid_register(d_fld2) ||
        !GPU::getInstance().is_valid_register(d_fld3)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return f1f2opf3_GPU(GPU::getInstance()[d_fld1], GPU::getInstance()[d_fld2], str_op, GPU::getInstance()[d_fld3]);
  }

  int
  f_to_s_GPU(
             string const & d_fld1,
             string const & str_op,
             string & str_result
             ) {
    if (!GPU::getInstance().is_valid_register(d_fld1)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return f_to_s_GPU(GPU::getInstance()[d_fld1], NULL, str_op, str_result);
  }

  int
  f1f2_to_s_GPU(
                string const & d_fld1,
                string const & d_fld2,
                string const & str_op,
                string & str_result
                ) {
    if (!GPU::getInstance().is_valid_register(d_fld1) ||
        !GPU::getInstance().is_valid_register(d_fld2)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    // note: f1f2_to_s_GPU is same as f_to_s_GPU with nn field for f
    // f1 (Q_I1_t) <--> nn (Q_I1_t), f2 <--> f
    // f1f2_to_s_GPU assumes there is NO nn field for f
    return f_to_s_GPU(GPU::getInstance()[d_fld2], &GPU::getInstance()[d_fld1], str_op, str_result);
  }

  //shift f1 by shift_amt (can be +ve or -ve) and get f2. fill in given value at shifted spots
  int
  f1_shift_f2_GPU(
                  string const & d_fld1,
                  string const & d_fld2,
                  string const & str_shift_amt,
                  string const & str_newval
                  /* new value inserted in null spots created by shift */
                  ) {
    int shift_amt;
    try {
      shift_amt = boost::lexical_cast<int>(str_shift_amt);
    } catch (boost::bad_lexical_cast const &) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    Q_I8_t fill_value;
    try {
      fill_value = boost::lexical_cast<Q_I8_t>(str_newval);
    } catch (boost::bad_lexical_cast const &) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    if (!GPU::getInstance().is_valid_register(d_fld1) ||
        !GPU::getInstance().is_valid_register(d_fld2)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return f1_shift_f2_GPU(GPU::getInstance()[d_fld1],
                           GPU::getInstance()[d_fld2],
                           shift_amt,
                           fill_value);
  }

  int
  count_GPU(
            string const & d_idx,
            string const & d_cnd,
            string const & d_dst,
            string const & safe_mode
            ) {
    if (!GPU::getInstance().is_valid_register(d_idx) ||
        (!d_cnd.empty() && !GPU::getInstance().is_valid_register(d_cnd)) ||
        !GPU::getInstance().is_valid_register(d_dst)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return count_GPU(GPU::getInstance()[d_idx],
                     (d_cnd.empty() ? NULL : &GPU::getInstance()[d_cnd]),
                     GPU::getInstance()[d_dst],
                     (safe_mode.empty() || (safe_mode != string("false"))));
  }

  int
  countf_GPU(
             string const & d_idx,
             string const & d_cnd,
             string const & d_val,
             string const & d_dst,
             string const & safe_mode
             ) {
    if (!GPU::getInstance().is_valid_register(d_idx) ||
        (!d_cnd.empty() && !GPU::getInstance().is_valid_register(d_cnd)) ||
        !GPU::getInstance().is_valid_register(d_val) ||
        !GPU::getInstance().is_valid_register(d_dst)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return countf_GPU(GPU::getInstance()[d_idx],
                      (d_cnd.empty() ? NULL : &GPU::getInstance()[d_cnd]),
                      GPU::getInstance()[d_val],
                      GPU::getInstance()[d_dst],
                      (safe_mode.empty() || (safe_mode != string("false"))));
  }

  //join - for all elements in vs that are in ls,
  // copy the corresponding value of vs to vd and mark nnd to 1.
  // for elements that are in vs and are not found in ls, mark vd to 0 and also nnd to 0.
  int
  join_GPU(
           string const & d_ls, // ls = link source
           string const & d_vs, // vs = value source
           string const & d_ld, // ld = link destination
           string const & d_vd, // vd = value destination
           string const & d_vd_nn, // vd_nn = nn field for value destination
           string const & str_op // <reg | min | max | sum | cnt | and | or>
           ) {
    if (!GPU::getInstance().is_valid_register(d_ls) ||
        !GPU::getInstance().is_valid_register(d_vs) ||
        !GPU::getInstance().is_valid_register(d_ld) ||
        !GPU::getInstance().is_valid_register(d_vd) ||
        !GPU::getInstance().is_valid_register(d_vd_nn)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return srt_join_GPU(GPU::getInstance()[d_ls],
                        GPU::getInstance()[d_vs],
                        GPU::getInstance()[d_ld],
                        str_op, GPU::getInstance()[d_vd],
                        GPU::getInstance()[d_vd_nn]);
  }

  // sorts in-place and will change the contents of the specified register
  int
  sort_GPU(
           string const & d_fld,
           string const & order
           ) {
    if (!GPU::getInstance().is_valid_register(d_fld)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return sort_GPU(GPU::getInstance()[d_fld], order);
  }

  int
  sortf1f2_GPU(
               string const & d_key,
               string const & d_val,
               string const & srttype
               ) {
    if (!GPU::getInstance().is_valid_register(d_key) ||
        !GPU::getInstance().is_valid_register(d_val)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return sortf1f2_GPU(GPU::getInstance()[d_key], GPU::getInstance()[d_val], srttype);
  }

  int
  mk_idx_GPU(
             string const & d_idx
             ) {
    if (!GPU::getInstance().is_valid_register(d_idx)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return mk_idx_GPU(GPU::getInstance()[d_idx]);
  }

  int
  permute_by_idx_GPU(
                     string const & d_data,
                     string const & d_idx,
                     string const & d_direction,
                     string const & d_result
                     ) {
    if (!GPU::getInstance().is_valid_register(d_data) ||
        !GPU::getInstance().is_valid_register(d_idx) ||
        !GPU::getInstance().is_valid_register(d_result)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return permute_by_idx_GPU(GPU::getInstance()[d_data],
                              GPU::getInstance()[d_idx],
                              (d_direction != string("reverse")),
                              GPU::getInstance()[d_result]);
  }

  int
  funnel_GPU(
             string const & d_key,
             string const & str_value,
             string const & d_same_session,
             string const & d_prev_funnel,
             string const & d_result
             ) {
    if (!GPU::getInstance().is_valid_register(d_key) ||
        !GPU::getInstance().is_valid_register(d_same_session) ||
        !GPU::getInstance().is_valid_register(d_prev_funnel) ||
        !GPU::getInstance().is_valid_register(d_result)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return funnel_GPU(GPU::getInstance()[d_key],
                      str_value,
                      GPU::getInstance()[d_same_session],
                      GPU::getInstance()[d_prev_funnel],
                      GPU::getInstance()[d_result]);
  }

  int
  funnel_count_GPU(
                   string const & d_key,
                   string const & str_distance,
                   string const & d_same_session,
                   string const & d_curr_funnel,
                   string const & d_result
                   ) {
    if (!GPU::getInstance().is_valid_register(d_key) ||
        !GPU::getInstance().is_valid_register(d_same_session) ||
        !GPU::getInstance().is_valid_register(d_curr_funnel) ||
        !GPU::getInstance().is_valid_register(d_result)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    size_t distance;
    try {
      distance = boost::lexical_cast<size_t>(str_distance);
    } catch (boost::bad_lexical_cast const &) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return funnel_count_GPU(GPU::getInstance()[d_key],
                            distance,
                            GPU::getInstance()[d_same_session],
                            GPU::getInstance()[d_curr_funnel],
                            GPU::getInstance()[d_result]);
  }

}
