#ifndef Q_F1F2OPF3_GPU_HPP
#define Q_F1F2OPF3_GPU_HPP

#include <string>

namespace QDB {
  class gpu_register;

  int f1f2opf3_GPU(gpu_register const & R1in,
                   gpu_register const & R2in,
                   std::string const & str_op,
                   gpu_register & Rout);
}

#endif
