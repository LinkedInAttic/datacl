#ifndef Q_INDEX_GPU_HPP
#define Q_INDEX_GPU_HPP

#include "q_types.hpp"

namespace QDB {
  class gpu_register;

  int mk_idx_GPU(gpu_register & Ridx);
  int permute_by_idx_GPU(gpu_register const & Rdata,
                         gpu_register const & Ridx,
                         bool bForward,
                         gpu_register & Rresult);
}

#endif
