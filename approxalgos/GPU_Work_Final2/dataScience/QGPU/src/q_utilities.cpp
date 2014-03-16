#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "q_utilities.hpp"

using std::string;
using std::cerr;
using std::endl;

namespace QDB {
  size_t
  get_sizeof(FLD_TYPE fldtype) {
    switch (fldtype) {
    case B  : return sizeof(fldtype_traits<I1>::q_type);
    case I1 : return sizeof(fldtype_traits<I1>::q_type);
    case I2 : return sizeof(fldtype_traits<I2>::q_type);
    case I4 : return sizeof(fldtype_traits<I4>::q_type);
    case I8 : return sizeof(fldtype_traits<I8>::q_type);
    case F4 : return sizeof(fldtype_traits<F4>::q_type);
    case F8 : return sizeof(fldtype_traits<F8>::q_type);
    case SC :
    case SV :
    default : return 0;
    }
  }

  string
  get_fldtype_string(FLD_TYPE fldtype) {
    switch (fldtype) {
    case B  : return string("B");
    case I1 : return string("I1");
    case I2 : return string("I2");
    case I4 : return string("I4");
    case I8 : return string("I8");
    case F4 : return string("F4");
    case F8 : return string("F8");
    case SC : return string("SC");
    case SV : return string("SV");
    default : return string("unknown");
    }
  }

  FLD_TYPE
  get_fldtype_enum(string const & fldtype) {
    if (fldtype == string("B")) {
      return B;
    } else if (fldtype == string("I1")) {
      return I1;
    } else if (fldtype == string("I2")) {
      return I2;
    } else if (fldtype == string("I4")) {
      return I4;
    } else if (fldtype == string("I8")) {
      return I8;
    } else if (fldtype == string("F4")) {
      return F4;
    } else if (fldtype == string("F8")) {
      return F8;
    } else if (fldtype == string("SC")) {
      return SC;
    } else if (fldtype == string("SV")) {
      return SV;
    } else {
      return undef_fldtype;
    }
  }

  MmappedData::MmappedData(string const & filename, off_t size)
    : m_X(NULL),
      m_nX(0),
      m_is_ok(false) {
    bool bWrite = (size >= 0);
    int fd = open(filename.c_str(), (bWrite ? O_RDWR|O_CREAT|O_TRUNC : O_RDONLY), S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if ((fd < 0) ||
        ((size > 0) && ((lseek(fd, size - 1, SEEK_SET) == -1) || (write(fd, "", 1) != 1)))) {
      cerr << "ERROR: failed to open file " << filename
           << " for " << (bWrite ? "writing" : "reading" ) << endl;
      whereami(__FILE__, __LINE__);
      return;
    }
    struct stat filestat;
    if ((fstat(fd, &filestat) == 0) && (filestat.st_size >= 0)) {
      if (filestat.st_size > 0) {
        m_nX = filestat.st_size;
        m_X = (char *)mmap(NULL, m_nX, (bWrite ? PROT_READ|PROT_WRITE : PROT_READ), MAP_SHARED, fd, 0);
      }
      m_is_ok = (filestat.st_size == 0) || (m_X != MAP_FAILED);
    }
    close(fd);
    if (!m_is_ok) {
      cerr << "ERROR: failure to map file " << filename
           << " for " << (size ? "writing" : "reading" ) << endl;
      whereami(__FILE__, __LINE__);
      return;
    }
  }

  MmappedData::~MmappedData() {
    if (m_is_ok && m_X && m_nX) {
      munmap(m_X, m_nX);
      m_X = NULL;
      m_nX = 0;
    }
  }
}
