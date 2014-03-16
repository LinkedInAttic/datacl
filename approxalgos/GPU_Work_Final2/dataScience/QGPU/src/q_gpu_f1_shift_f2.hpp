#ifndef Q_F1_SHIFT_F2_HPP
#define Q_F1_SHIFT_F2_HPP

#include "q_types.hpp"

namespace QDB {
  class gpu_register;

  int f1_shift_f2_GPU(gpu_register const & Rin,
                      gpu_register & Rout,
                      int n_shift,
                      Q_I8_t fill_value);

}

#endif
