#ifndef Q_SRT_JOIN_GPU_HPP
#define Q_SRT_JOIN_GPU_HPP

#include "q_types.hpp"
#include <string>

namespace QDB {
  class gpu_register;
  int srt_join_GPU(gpu_register const & RsrcLink,
                   gpu_register const & RsrcVal,
                   gpu_register const & RdstLink,
                   std::string const & str_op,
                   gpu_register & RdstVal,
                   gpu_register & RdstNn);
}

#endif
