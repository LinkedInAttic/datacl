#ifndef QGPU_SERVER_HPP
#define QGPU_SERVER_HPP

#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cuda.h>
#include <cuda_runtime_api.h>
#include "q_types.hpp"

//Function declarations

namespace QDB {

  // reg3[i] = reg1[i] op reg2[i]
  int
  f1f2opf3_GPU(
               std::string const & str_reg1,
               std::string const & str_reg2,
               std::string const & str_op,
               std::string const & str_reg3
               );

  // reg2[i] = reg1[i] op scalar
  int
  f1s1opf2_GPU(
               std::string const & str_reg1,
               std::string const & str_scalar,
               std::string const & str_op,
               std::string const & str_reg2
               );

  // return op(reg1)
  int
  f_to_s_GPU(
             std::string const & str_reg1,
             std::string const & str_op,
             std::string & str_result
             );

  // return op(reg1?reg2:0)
  int
  f1f2_to_s_GPU(
                std::string const & str_reg1,
                std::string const & str_reg2,
                std::string const & str_op,
                std::string & str_result
                );

  int
  f1opf2_GPU(
             std::string const & str_reg1,
             std::string const & str_reg2,
             std::string const & str_op,
             std::string const & str_opjresult
             );

  //shift f1 by shift_amt (can be +ve or -ve) and get f2. fill in given value at shifted spots
  int
  f1_shift_f2_GPU(
                  std::string const & str_reg1,
                  std::string const & str_reg2,
                  std::string const & str_shift_amt,
                  std::string const & str_value
                  );

  //Generate histogram (without value field)
  int
  count_GPU(
            std::string const & str_reg_data,
            std::string const & str_reg_cndl,
            std::string const & str_reg_out,
            std::string const & safe_mode
            );

  //Generate histogram (value fld)
  int
  countf_GPU(
             std::string const & str_reg_data,
             std::string const & str_reg_cndl,
             std::string const & str_reg_val,
             std::string const & str_reg_out,
             std::string const & safe_mode
             );

  //join - for all elements in vs that are in ls, copy the corresponding
  //value of vs to vd and mark nnd to 1. for elements that are in vs and
  //are not found in ls, mark vd to 0 and also nnd to 0.
  int
  join_GPU(
           std::string const & str_reg_ls,
           std::string const & str_reg_vs,
           std::string const & str_reg_ld,
           std::string const & str_reg_vd,
           std::string const & str_reg_nnd,
           std::string const & str_op
           );

  //sort (uses thrust)
  int
  sort_GPU(
           std::string const & d_fld,
           std::string const & order
           );

  int
  sortf1f2_GPU(
               std::string const & d_reg1,
               std::string const & d_reg2,
               std::string const & srttype
               );

  int
  mk_idx_GPU(
             std::string const & d_idx
             );

  int
  permute_by_idx_GPU(
                     std::string const & d_data,
                     std::string const & d_idx,
                     std::string const & direction,
                     std::string const & d_result
                     );

  int
  funnel_GPU(
             std::string const & d_key,
             std::string const & str_value,
             std::string const & d_same_session,
             std::string const & d_prev_funnel,
             std::string const & d_result
             );

  int
  funnel_count_GPU(
                   std::string const & d_key,
                   std::string const & str_distance,
                   std::string const & d_same_session,
                   std::string const & d_curr_funnel,
                   std::string const & d_result
                   );

  //prints reg contents to ascii file (for debugging)
  int
  print_data_to_ascii_file(
                           std::string const & str_reg_num,
                           std::string const & filename
                           );


};

#endif // QGPU_SERVER_HPP
