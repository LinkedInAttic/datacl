#ifndef Q_COUNT_GPU_HPP
#define Q_COUNT_GPU_HPP

#include "q_types.hpp"

namespace QDB {
  class gpu_register;

  int count_GPU(gpu_register const & Rsrc,
                gpu_register const * pRcond,
                gpu_register & Rdst,
                bool bSafeMode);

  int countf_GPU(gpu_register const & Rsrc,
                 gpu_register const * pRcond,
                 gpu_register const & Rval,
                 gpu_register & Rdst,
                 bool bSafeMode);
}

#endif
