#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "q_gpu_registers.hpp"
#include "q_types.hpp"
#include "q_utilities.hpp"

#define DEFAULT_GPU_PORT 8080

using std::string;
// using std::stringstream;
using std::ostringstream;
using std::ostream;
using std::ofstream;
using std::endl;
using std::map;
using std::cerr;
using std::endl;

using boost::shared_ptr;

static inline string safe_getenv(char const * name) {
  char * val = getenv(name);
  return (val ? string(val) : string(""));
}

#ifdef SUPPORT_LEGACY
namespace QDB_LEGACY {
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
}
#endif // SUPPORT_LEGACY

namespace QDB {
  // ************
  // gpu_register
  // ************

  gpu_register::gpu_register()
    : filename(),
      tbl(),
      h_fld(),
      d_fld(),
      nR(0),
      fldtype(undef_fldtype),
      pReg(),
      reg(NULL) {
  }

  int
  gpu_register::init(size_t in_nR, FLD_TYPE in_fldtype, string const & in_filename,
                     string const & in_tbl, string const & in_h_fld, string const & in_d_fld) {
    filename = in_filename;
    tbl = in_tbl;
    h_fld = in_h_fld;
    d_fld = in_d_fld;
    if (!pReg.get() || (nR != in_nR) || (fldtype != in_fldtype)) {
      pReg.reset((void *)(NULL));
      reg = NULL;
      nR = in_nR;
      fldtype = in_fldtype;
      size_t filesz = nR * get_sizeof(fldtype);
      void * mem;
      if (cudaMalloc(&mem, filesz) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      pReg = boost::shared_ptr<void>(mem, std::ptr_fun(cudaFree));
      reg = pReg.get();
    }
    return Q_PASS;
  }

  // list register properties/metadata
  int
  gpu_register::describe(
                         string & rsltbuf
                         ) const {
    ostringstream ss;
    ss << get_fldtype_string(fldtype)
       << "," << nR
       << "," << tbl
       << "," << h_fld
       << "," << d_fld;
    rsltbuf += ss.str();
    return Q_PASS;
  }

  int
  gpu_register::load(string const & in_filename) {
    if (!in_filename.empty()) {
      filename = in_filename;
    }

    if (filename.empty() || !boost::filesystem::exists(filename)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;      
    }

    MmappedData Data(filename);
    if (!Data.is_ok()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    if (!pReg.get() || (nR != (Data.nX() / get_sizeof(fldtype)))) {
      pReg.reset((void *)(NULL));
      reg = NULL;
      nR = Data.nX() / get_sizeof(fldtype);
    }

    if (nR) {
      if (!reg) {
        void * mem;
        if (cudaMalloc(&mem, Data.nX()) != cudaSuccess) {
          whereami(__FILE__, __LINE__);
          return Q_FAIL;
        }
        pReg = boost::shared_ptr<void>(mem, std::ptr_fun(cudaFree));
        reg = pReg.get();
      }
      if (cudaMemcpy(reg, Data.X(), Data.nX(), cudaMemcpyHostToDevice) != cudaSuccess) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    }

    return Q_PASS;
  }

  int
  gpu_register::store(string const & in_filename) const {
    if (!in_filename.empty()) {
      filename = in_filename;
    }
    MmappedData Data(filename, nR * get_sizeof(fldtype));
    if (!Data.is_ok()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    if (cudaMemcpy(Data.X(), reg, Data.nX(), cudaMemcpyDeviceToHost) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return Q_PASS;
  }

  // ************
  // print_helper
  // ************

  struct __print_helper_interface {
    virtual int operator()(ostream & os, string const & separator,
                           gpu_register const & reg, size_t nelem, bool bTail) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
  };

  template <typename T>
  struct __print_helper : public __print_helper_interface {
    virtual int operator()(ostream & os, string const & separator,
                           gpu_register const & reg, size_t nelem, bool bTail) {
      if (reg.nR == 0) {
        return Q_FAIL;
      }
      nelem = nelem ? std::min(size_t(reg.nR), nelem) : size_t(reg.nR);
      size_t numbytes = nelem * sizeof(T);
      void * dst = malloc(numbytes);
      T const * src = (T *)(reg.reg);
      if (bTail) {
        src += (reg.nR - nelem);
      }
      cudaMemcpy(dst, (void *)(src), numbytes, cudaMemcpyDeviceToHost);
      T * i = (T *)(dst);
      T * iEnd = i + nelem;
      typedef typename qtype_traits<T>::print_type print_type;
      os << static_cast<print_type>(*i);
      while (++i != iEnd) {
        os << separator << static_cast<print_type>(*i);
      }
      free(dst);
      return Q_PASS;
    }
  };


  struct print_helper {
    int operator()(ostream & os, string const & separator,
                   gpu_register const & reg, size_t nelem = 0, bool bTail = false) {
      if (op.find(reg.fldtype) == op.end()) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      op[reg.fldtype]->operator()(os, separator, reg, nelem, bTail);
      return Q_PASS;
    }

    print_helper() {
      addop<I1>();
      addop<I2>();
      addop<I4>();
      addop<I8>();
      addop<F4>();
      addop<F8>();
    }

    template<FLD_TYPE fldtype>
    inline void addop() {
      typedef typename fldtype_traits<fldtype>::q_type q_type;
      op[fldtype] = shared_ptr<__print_helper_interface>
        (static_cast<__print_helper_interface *>(new __print_helper<q_type>()));
    }

    map<FLD_TYPE, shared_ptr<__print_helper_interface> > op;
  };

  static print_helper s_print_helper;

  // ***
  // GPU
  // ***

  //get the free memory in bytes available on the GPU
  int
  GPU::list_free_mem(string & str_free_mem) const {
    size_t available, total;
    if (cudaMemGetInfo(&available, &total) != cudaSuccess) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    str_free_mem += boost::lexical_cast<string>(available);
    return Q_PASS;
  }

  bool GPU::is_valid_register(string const & reg_name) const {
    return (gpu_reg.find(reg_name) != gpu_reg.end());
  }

  // returns the number of registers
  int
  GPU::list_num_registers(string & str_num_reg) const {
    str_num_reg = boost::lexical_cast<string>(gpu_reg.size());
    return Q_PASS;
  }

  void
  GPU::list_registers(string & str_free_reg) const {
    if (!gpu_reg.empty()) {
      ostringstream ss;
      map<string, gpu_register>::const_iterator i = gpu_reg.begin();
      ss << i->first;
      for (++i; i != gpu_reg.end(); ++i) {
        ss << ":" << i->first;
      }
      str_free_reg += ss.str();
    }
  }

  // list register properties/metadata
  int
  GPU::describe_register(string const & str_reg, string & rsltbuf) const {
    map<string, gpu_register>::const_iterator iFound = gpu_reg.find(str_reg);
    if (iFound == gpu_reg.end()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    return iFound->second.describe(rsltbuf);
  }

  //Prints the (first or last) N data elements of the specified register
  int
  GPU::print_reg_data (string const & str_reg, string const & str_nelem,
                       string const & str_head_or_tail, string & rsltbuf) {
    if (str_head_or_tail != "head" &&
        str_head_or_tail != "tail") {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    map<string, gpu_register>::const_iterator iFound = gpu_reg.find(str_reg);
    if (iFound == gpu_reg.end()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    int nelem = atoi(str_nelem.c_str());
    if (( nelem <= 0 ) || ( nelem > iFound->second.nR )) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    ostringstream sout;
    if (s_print_helper(sout, "\t", iFound->second, nelem, (str_head_or_tail == "tail")) != Q_PASS) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    rsltbuf += sout.str();

    return Q_PASS;
  }

  //prints data in given register to the given ascii file. Only used for debugging. currently assuming int datatype only.
  int
  GPU::print_data_to_ascii_file (string const & str_reg,
                                 string const & filename) {
    ofstream os(filename.c_str());
    if (s_print_helper(os, "\n", gpu_reg[str_reg], 0, false) != Q_PASS) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    os << endl;
    return Q_PASS;
  }

  //adds a new field from the given file, and also sets appropriate metadata
  int
  GPU::add_fld(
               string const & str_filesz,
               string const & str_nR,
               string const & str_fldtype,
               string const & filename,
               string const & tbl,
               string const & h_fld,
               string const & d_fld
               ) {
    FLD_TYPE fldtype = get_fldtype_enum(str_fldtype);
    if ((fldtype != I1) && (fldtype != I2) && (fldtype != I4) && (fldtype != I8) &&
        (fldtype != F4) && (fldtype != F8)) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    size_t nR;
    if (!str_nR.empty()) {
      try {
        nR = boost::lexical_cast<size_t>(str_nR);
      } catch (boost::bad_lexical_cast const &) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    } else if (!str_filesz.empty()) {
      try {
        size_t filesz = boost::lexical_cast<size_t>(str_nR);
        nR = filesz / get_sizeof(fldtype);
      } catch (boost::bad_lexical_cast const &) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    } else if (!filename.empty() && boost::filesystem::exists(filename) && boost::filesystem::file_size(filename)) {
      size_t filesz = boost::filesystem::file_size(filename);
      nR = filesz / get_sizeof(fldtype);
    } else {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }

    if (gpu_reg[d_fld].init(nR, fldtype, filename, tbl, h_fld, d_fld) != Q_PASS) {
      del_fld(d_fld);
      whereami(__FILE__, __LINE__);
      return Q_FAIL;      
    }

#ifdef SUPPORT_LEGACY
    create_legacy_mmap_file();
#endif // SUPPORT_LEGACY

    return Q_PASS;
  }

  //deletes given field
  void
  GPU::del_fld(
               string const & d_fld
               ) {
    map<string, gpu_register>::iterator iFound = gpu_reg.find(d_fld);
    if (iFound != gpu_reg.end()) {
      gpu_reg.erase(iFound);
#ifdef SUPPORT_LEGACY
      create_legacy_mmap_file();
#endif // SUPPORT_LEGACY
    } else if (d_fld == string("ALL_FLDS")) {
      gpu_reg.clear();
#ifdef SUPPORT_LEGACY
      create_legacy_mmap_file();
#endif // SUPPORT_LEGACY
    }
  }

  // transfers data from file to GPU register, and sets metadata
  int
  GPU::load(
            string const & str_filesz,
            string const & str_nR,
            string const & str_fldtype,
            string const & filename,
            string const & tbl,
            string const & h_fld,
            string const & d_fld
            ) {
    if ((add_fld(str_filesz, str_nR, str_fldtype, filename, tbl, h_fld, d_fld) != Q_PASS) ||
        (gpu_reg[d_fld].load() != Q_PASS)) {
      del_fld(d_fld);
      return Q_FAIL;
    }
    return Q_PASS;
  }

  // transfers data from register on device to file
  int
  GPU::store(string const & filename,
             string const & d_fld) {
    map<string, gpu_register>::const_iterator iFound = gpu_reg.find(d_fld);
    if (iFound == gpu_reg.end()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    iFound->second.store(filename);
#ifdef SUPPORT_LEGACY
    create_legacy_mmap_file();
#endif // SUPPORT_LEGACY
    return Q_PASS;
  }

  int
  GPU::swap_flds(string const & d_fld1,
                 string const & d_fld2) {
    bool bValid1 = is_valid_register(d_fld1);
    bool bValid2 = is_valid_register(d_fld2);
    if (bValid1 && bValid2) {
      std::swap(gpu_reg[d_fld1], gpu_reg[d_fld2]);
    } else if (bValid1) {
      gpu_reg[d_fld2] = gpu_reg[d_fld1];
      del_fld(d_fld1);
    } else if (bValid2) {
      gpu_reg[d_fld1] = gpu_reg[d_fld2];
      del_fld(d_fld2);
    } else {
      return Q_FAIL;
    }
    return Q_PASS;
  }

  gpu_register &
  GPU::operator[](string const & str_reg) {
    return gpu_reg[str_reg];
  }

  GPU & GPU::getInstance() {
    static GPU instance;
    return instance;
  }

  GPU::GPU()
    : gpu_server(safe_getenv("Q_GPU_SERVER")),
      docroot(safe_getenv("Q_DOCROOT")),
      data_dir(safe_getenv("Q_DATA_DIR")),
      cwd(),
      gpu_port(DEFAULT_GPU_PORT),
      ddir_id(-1),
      alt_ddir_id(-1),
      gpu_reg() {
    string port = safe_getenv("Q_GPU_PORT");
    if (!port.empty()) {
      gpu_port = boost::lexical_cast<int>(port);
    }
    char * cwd_tmp = getcwd(NULL, 0);
    if (cwd_tmp) {
      cwd = string(cwd_tmp);
      free(cwd_tmp);
    }
  }

#ifdef SUPPORT_LEGACY
  int
  GPU::create_legacy_mmap_file() const {
    string gpu_meta_file = docroot + string("/docroot.gpu");
    MmappedData Data(gpu_meta_file, sizeof(QDB_LEGACY::GPU_REG_TYPE) * gpu_reg.size());
    if (!Data.is_ok()) {
      whereami(__FILE__, __LINE__);
      return Q_FAIL;
    }
    QDB_LEGACY::GPU_REG_TYPE * pRegOld = (QDB_LEGACY::GPU_REG_TYPE *)(Data.X());
    for (map<string, gpu_register>::const_iterator i = gpu_reg.begin(), iEnd = gpu_reg.end(); i != iEnd; ++i, ++pRegOld) {
      pRegOld->reg = NULL;
      pRegOld->filesz = i->second.nR * get_sizeof(i->second.fldtype);
      pRegOld->is_busy = true;
      pRegOld->nR = i->second.nR;
      pRegOld->fldtype = QDB_LEGACY::FLD_TYPE(i->second.fldtype);
      if (strlen(i->second.filename.c_str()) >= QDB_LEGACY::MAX_LEN_FILE_NAME) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      strncpy(pRegOld->filename, i->second.filename.c_str(), QDB_LEGACY::MAX_LEN_FILE_NAME);
      if (strlen(i->second.tbl.c_str()) >= QDB_LEGACY::MAX_LEN_TBL_NAME) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      strncpy(pRegOld->tbl, i->second.tbl.c_str(), QDB_LEGACY::MAX_LEN_TBL_NAME);
      if (strlen(i->second.h_fld.c_str()) >= QDB_LEGACY::MAX_LEN_FLD_NAME) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      strncpy(pRegOld->h_fld, i->second.h_fld.c_str(), QDB_LEGACY::MAX_LEN_FLD_NAME);
      if (strlen(i->second.d_fld.c_str()) >= QDB_LEGACY::MAX_LEN_FLD_NAME) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      strncpy(pRegOld->d_fld, i->second.d_fld.c_str(), QDB_LEGACY::MAX_LEN_FLD_NAME);
    }
    return  Q_PASS;
  }
#endif // SUPPORT_LEGACY
}
