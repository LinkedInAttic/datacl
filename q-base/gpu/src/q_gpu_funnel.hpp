#ifndef Q_FUNNEL_GPU_HPP
#define Q_FUNNEL_GPU_HPP

#include <string>
#include "q_types.hpp"

namespace QDB {
  class gpu_register;

  int funnel_GPU(gpu_register const & Rkey,
                 std::string const & str_value,
                 gpu_register const & Rcond_curr,
                 gpu_register const & Rcond_prev,
                 gpu_register & Rresult);

  int funnel_count_GPU(gpu_register const & Rkey,
                       size_t distance,
                       gpu_register const & Rsame_session,
                       gpu_register const & Rcurr_funnel,
                       gpu_register & Rresult);
}

#endif
