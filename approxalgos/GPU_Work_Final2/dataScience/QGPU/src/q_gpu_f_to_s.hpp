#ifndef Q_F_TO_S_GPU_HPP
#define Q_F_TO_S_GPU_HPP

#include "q_types.hpp"
#include <string>

namespace QDB {

  class gpu_register;

  int f_to_s_GPU(gpu_register const & R,
                 gpu_register const * pRnn,
                 std::string const & str_op,
                 std::string & str_result);

}

#endif
