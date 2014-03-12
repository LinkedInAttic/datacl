#ifndef Q_SORT_GPU_HPP
#define Q_SORT_GPU_HPP

#include <string>
#include "q_types.hpp"

namespace QDB {
  class gpu_register;
  int sort_GPU(gpu_register & R, std::string const & srttype);
  int sortf1f2_GPU(gpu_register & Rkey, gpu_register & Rval, std::string const & srttype);
}

#endif
