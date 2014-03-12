#ifndef Q_GPU_REGISTERS_HPP
#define Q_GPU_REGISTERS_HPP

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>
#include "q_types.hpp"

namespace QDB {

  class gpu_register {
  public:
    gpu_register();
    int init(size_t nR, FLD_TYPE fldtype, std::string const & filename,
             std::string const & tbl, std::string const & h_fld, std::string const & d_fld);
    int describe(std::string & rsltbuf) const;

    int load(std::string const & filename = std::string(""));
    int store(std::string const & filename = std::string("")) const;

    mutable std::string filename;
    std::string tbl;
    std::string h_fld;
    std::string d_fld;
    size_t nR;
    FLD_TYPE fldtype;
    boost::shared_ptr<void> pReg; // points to data on device
    void * reg; // raw ptr for convenience
  };

  class GPU {
  public:
    static GPU & getInstance();

    bool is_valid_register(std::string const & reg_name) const;

    int list_free_mem(std::string & str_free_mem) const;
    int list_num_registers(std::string & str_num_reg) const;
    // returns a colon separated list of registers
    void list_registers(std::string & str_free_reg) const;
    // list register properties/metadata
    int describe_register(std::string const & d_fld, std::string & rsltbuf) const;


    // Prints the (first or last) N data elements of the specified register
    int print_reg_data(std::string const & d_fld, std::string const & str_nelem,
                        std::string const & str_head_or_tail, std::string & rsltbuf);

    int print_data_to_ascii_file(std::string const & str_reg,
                                 std::string const & str_filename);

    // adds a new field
    int
    add_fld(std::string const & str_filesz,
            std::string const & str_nR,
            std::string const & str_fldtype,
            std::string const & str_filename,
            std::string const & str_tbl,
            std::string const & str_h_fld,
            std::string const & str_d_fld);

    //deletes given field
    void
    del_fld(std::string const & d_fld);

    // transfers data from file to GPU register
    int
    load(std::string const & str_filesz,
         std::string const & str_nR,
         std::string const & str_fldtype,
         std::string const & str_filename,
         std::string const & str_tbl,
         std::string const & str_h_fld,
         std::string const & str_d_fld);

    // transfers data from register on device to file
    int store(std::string const & d_fld,
              std::string const & str_filename);

    int swap_flds(std::string const & d_fld1,
                  std::string const & d_fld2);

    gpu_register & operator[](std::string const & str_reg);

#define SUPPORT_LEGACY
#ifdef SUPPORT_LEGACY
    int
    create_legacy_mmap_file() const;
#endif // SUPPORT_LEGACY

  private:
    GPU();
    GPU(GPU const &);
    GPU & operator=(GPU const &);

  public:
    std::string gpu_server;
    std::string docroot;
    std::string data_dir;
    std::string cwd;
    int  gpu_port;

    int ddir_id;
    int alt_ddir_id;

  private:
    std::map<std::string, gpu_register> gpu_reg;
  };

}

#endif // Q_GPU_REGISTERS_HPP
