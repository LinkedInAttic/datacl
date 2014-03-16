#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>

using namespace std;

typedef enum _FLD_TYPE {
  undef_fldtype,
  B,
  I1,
  I2,
  I4,
  I8,
  F4,
  F8,
  SC, /* raw string. Must be fixed length. len must be set */
  SV, /* Variable length string. dict_tbl_id must be set. */
} FLD_TYPE;

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

const size_t MAX_LEN_FILE_NAME = 511;
const size_t MAX_LEN_DIR_NAME = 511;
const size_t MAX_LEN_FLD_NAME = 31;
const size_t MAX_LEN_TBL_NAME = 31;

typedef struct _GPU_REG_TYPE {
  void * reg; // points to data on device
  size_t filesz;
  bool is_busy;
  long long nR;
  FLD_TYPE fldtype;
  char filename[MAX_LEN_DIR_NAME+63];
  char tbl[MAX_LEN_TBL_NAME+1];
  char h_fld[MAX_LEN_FLD_NAME+1];
  char d_fld[MAX_LEN_FLD_NAME+1];
} GPU_REG_TYPE;

int main(int argc, char const * argv[]) {
  if (argc < 1) {
    cerr << "usage: " << argv[0] << " filename" << endl;
    return -1;
  }
  struct stat filestat;
  int fd = open(argv[1], O_RDONLY);
  if ((fd < 0) || (fstat(fd, &filestat) != 0) && (filestat.st_size < 0)) {
    cerr << "ERROR: failed to open file: " << argv[1] << endl;
    return -1;
  }
  if (filestat.st_size % sizeof(GPU_REG_TYPE)) {
    cerr << "ERROR: filesize is not an exact multiple of the GPU register type" << endl;
  }
  GPU_REG_TYPE const * pReg = (GPU_REG_TYPE const *)mmap(NULL, filestat.st_size, PROT_READ, MAP_SHARED, fd, 0);
  for (GPU_REG_TYPE const * i = pReg, * iEnd = pReg + (filestat.st_size / sizeof(GPU_REG_TYPE)); i != iEnd; ++i) {
    cerr << "Register:"
         << "\n\tfilesz=" << i->filesz
         << "\n\tis_busy=" << (i->is_busy ? "busy" : "free")
         << "\n\tnR=" << i->nR
         << "\n\tfldtype=" << get_fldtype_string(i->fldtype)
         << "\n\tfilename=" << string(i->filename)
         << "\n\ttbl=" << string(i->tbl)
         << "\n\th_fld=" << string(i->h_fld)
         << "\n\td_fld=" << string(i->d_fld)
         << endl;
  }
  close(fd);
  munmap((void *)pReg, filestat.st_size);
  return 0;
}
