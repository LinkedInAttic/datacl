#include <memory>
#include <vector>
#include <string>
#include <map>
#include <iostream>

#include "q_types.hpp"
#include "q_gpu_registers.hpp"
#include "q_gpu_server.hpp"

using std::string;
using std::vector;
using std::map;
using std::cerr;
using std::endl;
using std::shared_ptr;

namespace QDB {

  class GpuCommand {
  public:
    GpuCommand(string const & name = "invalid_cmd",
               bool bVarArgs = false)
      : m_cmdName(name),
        m_cmdArgs(),
        m_bVarArgs(bVarArgs) {
    }
    int operator()(vector<string> const & args,
                   string & result) {
      vector<string> argv;
      if (get_argv_ordered(args, argv) != Q_PASS) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      int retval = execute_command(argv, result);
      cudaDeviceSynchronize();
      return retval;
    }

    virtual int execute_command(vector<string> const & args,
                                string & result) {
      return Q_FAIL;
    }
    string const & getName() const {
      return m_cmdName;
    }

  protected:
    int get_argv_ordered(vector<string> const & args,
                         vector<string> & argvals) {
      // the first token is the name of the operation
      if ((args.size() < 2) || (args[0] != string("gq")) || (args[1] != m_cmdName) ||
          (!m_bVarArgs && ((args.size() - 2) != m_cmdArgs.size()))) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      argvals.clear();
      if (args.size() > 2) {
        argvals.resize(m_cmdArgs.size());
        for (vector<string>::const_iterator i = args.begin() + 2, iEnd = args.end(); i != iEnd; ++i) {
          size_t eq_loc = i->find('=');
          if (eq_loc == string::npos) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
          string arg = i->substr(0, eq_loc);
          string val = i->substr(eq_loc + 1);
          int index = (m_cmdArgs.size() == 0 ? -1 :
                       m_cmdArgs.find(arg) == m_cmdArgs.end() ? - 1 :
                       m_cmdArgs[arg]);
          if ((index < 0) || (index >= int(m_cmdArgs.size())) ||
              (!m_bVarArgs && (val.empty() || !argvals[index].empty()))) {
            whereami(__FILE__, __LINE__);
            return Q_FAIL;
          }
          argvals[index] = val;
        }
      }
      return Q_PASS;
    }
    string m_cmdName;
    map<string, size_t> m_cmdArgs;
    bool m_bVarArgs;
  };

  struct GpuCommand_list_free_mem : public GpuCommand {
    GpuCommand_list_free_mem()
      : GpuCommand("list_free_mem") {
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().list_free_mem(result);
    }
  };

  struct GpuCommand_list_num_registers : public GpuCommand {
    GpuCommand_list_num_registers()
      : GpuCommand("list_num_registers") {
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().list_num_registers(result);
    }
  };

  struct GpuCommand_list_free_registers : public GpuCommand {
    GpuCommand_list_free_registers()
      : GpuCommand("list_free_registers") {
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      // does nothing - registers are deleted when freed
      // list_registers(result, "free");
      return Q_PASS;
    }
  };

  struct GpuCommand_list_used_registers : public GpuCommand {
    GpuCommand_list_used_registers ()
      : GpuCommand("list_used_registers") {
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      GPU::getInstance().list_registers(result);
      return Q_PASS;
    }
  };

  struct GpuCommand_describe_register : public GpuCommand {
    GpuCommand_describe_register()
      : GpuCommand("describe_register") {
      m_cmdArgs["d_fld"] = 0;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().describe_register(args[0], result);
    }
  };

  struct GpuCommand_print_reg_data : public GpuCommand {
    GpuCommand_print_reg_data()
      : GpuCommand("print_reg_data") {
      m_cmdArgs["d_fld"] = 0;
      m_cmdArgs["n"] = 1;
      m_cmdArgs["mode"] = 2;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().print_reg_data(args[0], args[1], args[2], result);
    }
  };

  struct GpuCommand_load : public GpuCommand {
    GpuCommand_load()
      : GpuCommand("load", true) {
      // only one of filesz or nR is needed.
      // if both are provided, then nR is used
      m_cmdArgs["filesz"] = 0;
      m_cmdArgs["nR"] = 1;
      m_cmdArgs["fldtype"] = 2;
      m_cmdArgs["filename"] = 3;
      m_cmdArgs["tbl"] = 4;
      m_cmdArgs["h_fld"] = 5;
      m_cmdArgs["d_fld"] = 6;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().load(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    }
  };

  struct GpuCommand_store : public GpuCommand {
    GpuCommand_store()
      : GpuCommand("store", true) {
      // filename is optional if it was specified during the load/add_fld.
      m_cmdArgs["filename"] = 0;
      m_cmdArgs["d_fld"] = 1;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().store(args[0], args[1]);
    }
  };

  struct GpuCommand_add_fld : public GpuCommand {
    GpuCommand_add_fld()
      : GpuCommand("add_fld", true) {
      m_cmdArgs["filesz"] = 0;
      m_cmdArgs["nR"] = 1;
      m_cmdArgs["fldtype"] = 2;
      // filename is  optional. can be specified during store instead.
      m_cmdArgs["filename"] = 3;
      m_cmdArgs["tbl"] = 4;
      m_cmdArgs["h_fld"] = 5;
      m_cmdArgs["d_fld"] = 6;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().add_fld(args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    }
  };

  struct GpuCommand_del_fld : public GpuCommand {
    GpuCommand_del_fld()
      : GpuCommand("del_fld") {
      m_cmdArgs["d_fld"] = 0;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      GPU::getInstance().del_fld(args[0]);
      return Q_PASS;
    }
  };

  struct GpuCommand_swap_flds : public GpuCommand {
    GpuCommand_swap_flds()
      : GpuCommand("swap_flds") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      GPU::getInstance().swap_flds(args[0], args[1]);
      return Q_PASS;
    }
  };

  struct GpuCommand_f1s1opf2 : public GpuCommand {
    GpuCommand_f1s1opf2()
      : GpuCommand("f1s1opf2") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["scalar"] = 1;
      m_cmdArgs["op"] = 2;
      m_cmdArgs["d_fld2"] = 3;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return f1s1opf2_GPU(args[0], args[1], args[2], args[3]);
    }
  };

  struct GpuCommand_f1f2opf3 : public GpuCommand {
    GpuCommand_f1f2opf3()
      : GpuCommand("f1f2opf3") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
      m_cmdArgs["op"] = 2;
      m_cmdArgs["d_fld3"] = 3;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return f1f2opf3_GPU(args[0], args[1], args[2], args[3]);
    }
  };

  struct GpuCommand_f_to_s : public GpuCommand {
    GpuCommand_f_to_s()
      : GpuCommand("f_to_s", true /* aliasing --> variable args */) {
      m_cmdArgs["d_fld1"] = 0; // for backward compatibility
      m_cmdArgs["d_fld"] = 0;
      m_cmdArgs["op"] = 1;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return f_to_s_GPU(args[0], args[1], result);
    }
  };

  struct GpuCommand_f1f2_to_s : public GpuCommand {
    GpuCommand_f1f2_to_s()
      : GpuCommand("f1f2_to_s") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
      m_cmdArgs["op"] = 2;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return f1f2_to_s_GPU(args[0], args[1], args[2], result);
    }
  };

  struct GpuCommand_f1opf2 : public GpuCommand {
    GpuCommand_f1opf2()
      : GpuCommand("f1opf2", true /* variable arguments */) {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
      m_cmdArgs["op"] = 2;
      m_cmdArgs["val"] = 3;
      m_cmdArgs["newval"] = 4;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      if (args[2] == "shift") {
        return f1_shift_f2_GPU(args[0], args[1], args[3], args[4]);
      } else {
        cerr << "ERROR: only f1opf2 shift is currently supported" << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    }
  };

  struct GpuCommand_f1_shift_f2 : public GpuCommand {
    GpuCommand_f1_shift_f2()
      : GpuCommand("f1_shift_f2") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
      m_cmdArgs["shift_amt"] = 2;
      m_cmdArgs["newval"] = 3;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return f1_shift_f2_GPU(args[0], args[1], args[2], args[3]);
    }
  };

  struct GpuCommand_count : public GpuCommand {
    GpuCommand_count()
      : GpuCommand("count", true /* variable arguments */) {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_cfld"] = 1;
      m_cmdArgs["d_fld2"] = 2;
      m_cmdArgs["safe_mode"] = 3;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return count_GPU(args[0], args[1], args[2], args[3]);
    }
  };

  struct GpuCommand_count_f : public GpuCommand {
    GpuCommand_count_f()
      : GpuCommand("count_f", true /* variable arguments */) {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_cfld"] = 1;
      m_cmdArgs["d_fld2"] = 2;
      m_cmdArgs["d_out"] = 3;
      m_cmdArgs["safe_mode"] = 4;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return countf_GPU(args[0], args[1], args[2], args[3], args[4]);
    }
  };

  struct GpuCommand_join : public GpuCommand {
    GpuCommand_join()
      : GpuCommand("join") {
      m_cmdArgs["d_ls"] = 0;
      m_cmdArgs["d_vs"] = 1;
      m_cmdArgs["d_ld"] = 2;
      m_cmdArgs["d_vd"] = 3;
      m_cmdArgs["d_vd_nn"] = 4;
      m_cmdArgs["op"] = 5;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return join_GPU(args[0], args[1], args[2], args[3], args[4], args[5]);
    }
  };

  struct GpuCommand_fop : public GpuCommand {
    GpuCommand_fop()
      : GpuCommand("fop") {
      m_cmdArgs["op"] = 0;
      m_cmdArgs["d_fld"] = 1;
      m_cmdArgs["order"] = 2;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      if (args[0] == string("sort")) {
        return sort_GPU(args[1], args[2]);
      } else {
        cerr << "ERROR: only fop sort [asc|dsc] is currently supported" << endl;
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
    }
  };

  struct GpuCommand_sort : public GpuCommand {
    GpuCommand_sort()
      : GpuCommand("sort", true) {
      m_cmdArgs["d_fld"] = 0;
      m_cmdArgs["order"] = 1;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return sort_GPU(args[0], (args[1].empty() ? string("asc") : args[1]));
    }
  };

  struct GpuCommand_sortf1f2 : public GpuCommand {
    GpuCommand_sortf1f2()
      : GpuCommand("sortf1f2") {
      m_cmdArgs["d_fld1"] = 0;
      m_cmdArgs["d_fld2"] = 1;
      m_cmdArgs["srttype"] = 2;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return sortf1f2_GPU(args[0], args[1], args[2]);
    }
  };

  struct GpuCommand_mk_idx : public GpuCommand {
    GpuCommand_mk_idx()
      : GpuCommand("mk_idx") {
      m_cmdArgs["d_fld"] = 0;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return mk_idx_GPU(args[0]);
    }
  };

  struct GpuCommand_permute_by_idx : public GpuCommand {
    GpuCommand_permute_by_idx()
      : GpuCommand("permute_by_idx", true) {
      m_cmdArgs["d_data"] = 0;
      m_cmdArgs["d_idx"] = 1;
      m_cmdArgs["direction"] = 2;
      m_cmdArgs["d_result"] = 3;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return permute_by_idx_GPU(args[0], args[1], args[2], args[3]);
    }
  };

  struct GpuCommand_funnel : public GpuCommand {
    GpuCommand_funnel()
      : GpuCommand("funnel") {
      m_cmdArgs["d_key"] = 0;
      m_cmdArgs["d_value"] = 1;
      m_cmdArgs["d_same_session"] = 2;
      m_cmdArgs["d_prev_funnel"] = 3;
      m_cmdArgs["d_result"] = 4;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return funnel_GPU(args[0], args[1], args[2], args[3], args[4]);
    }
  };

  struct GpuCommand_funnel_count : public GpuCommand {
    GpuCommand_funnel_count()
      : GpuCommand("funnel_count") {
      m_cmdArgs["d_key"] = 0;
      m_cmdArgs["d_distance"] = 1;
      m_cmdArgs["d_same_session"] = 2;
      m_cmdArgs["d_curr_funnel"] = 3;
      m_cmdArgs["d_result"] = 4;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return funnel_count_GPU(args[0], args[1], args[2], args[3], args[4]);
    }
  };

  struct GpuCommand_print_data_to_ascii_file : public GpuCommand {
    GpuCommand_print_data_to_ascii_file()
      : GpuCommand("print_data_to_ascii_file") {
      m_cmdArgs["reg"] = 0;
      m_cmdArgs["filename"] = 1;
    }
    int execute_command(vector<string> const & args,
                        string & result) {
      return GPU::getInstance().print_data_to_ascii_file(args[0], args[1]);
    }
  };

  class GPUCommands {
  public:
    GPUCommands()
      : m_cmd() {
      register_cmd(new GpuCommand_list_free_mem);
      register_cmd(new GpuCommand_list_num_registers);
      register_cmd(new GpuCommand_list_free_registers);
      register_cmd(new GpuCommand_list_used_registers);
      register_cmd(new GpuCommand_describe_register);
      register_cmd(new GpuCommand_print_reg_data);
      register_cmd(new GpuCommand_load);
      register_cmd(new GpuCommand_store);
      register_cmd(new GpuCommand_add_fld);
      register_cmd(new GpuCommand_del_fld);
      register_cmd(new GpuCommand_swap_flds);
      register_cmd(new GpuCommand_f1s1opf2);
      register_cmd(new GpuCommand_f1f2opf3);
      register_cmd(new GpuCommand_f_to_s);
      register_cmd(new GpuCommand_f1f2_to_s);
      register_cmd(new GpuCommand_f1opf2);
      register_cmd(new GpuCommand_f1_shift_f2);
      register_cmd(new GpuCommand_count);
      register_cmd(new GpuCommand_count_f);
      register_cmd(new GpuCommand_join);
      register_cmd(new GpuCommand_fop);
      register_cmd(new GpuCommand_sort);
      register_cmd(new GpuCommand_mk_idx);
      register_cmd(new GpuCommand_permute_by_idx);
      register_cmd(new GpuCommand_funnel);
      register_cmd(new GpuCommand_funnel_count);
      register_cmd(new GpuCommand_sortf1f2);
      register_cmd(new GpuCommand_print_data_to_ascii_file);
    }

    template <typename T>
    void register_cmd(T * pCmd) {
      m_cmd[pCmd->getName()] = shared_ptr<GpuCommand>(static_cast<GpuCommand *>(pCmd));
    }

    int execute_cmd(vector<string> const & args, string & result) {
      if ((args.size() < 2) || (args[0] != string("gq"))) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      if (m_cmd.find(args[1]) == m_cmd.end()) {
        whereami(__FILE__, __LINE__);
        return Q_FAIL;
      }
      return m_cmd[args[1]]->operator()(args, result);
    }

  private:
    map<string, shared_ptr<GpuCommand> > m_cmd;
  };

  int
  qgpu(
       vector<string> const & args,
       string & result
       ) {

    static GPUCommands s_commands;
    return s_commands.execute_cmd(args, result);
  }

};
