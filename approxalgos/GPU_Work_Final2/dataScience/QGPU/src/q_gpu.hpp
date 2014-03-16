#ifndef QGPU_HPP
#define QGPU_HPP

#include <vector>
#include <string>

namespace QDB {

  int
  qgpu(
       std::vector<std::string> const & args,
       std::string & rslt_buf
       );

}

#endif
