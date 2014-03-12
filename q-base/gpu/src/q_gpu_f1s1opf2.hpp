#ifndef Q_F1S1OPF2_GPU_HPP
#define Q_F1S1OPF2_GPU_HPP

#include "q_types.hpp"
#include <string>

namespace QDB {
  class gpu_register;

  int f1s1opf2_GPU(gpu_register const & Rin,
                   std::string const & str_scalar,
                   std::string const & str_op,
                   gpu_register & Rout);
}

#endif
