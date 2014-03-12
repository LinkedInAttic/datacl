#include "qtypes.h"
#include "auxil.h"
extern int  g_num_cores; // number of cores on this machine
extern char g_gpu_server[MAX_LEN_SERVER_NAME+1];
extern char g_docroot[MAX_LEN_DIR_NAME+1];
extern int  g_gpu_port;
extern char g_cwd[MAX_LEN_DIR_NAME+1];
extern FILE *g_ofp;
extern bool g_use_ram_dir;
// START FUNC DECL
int
  q_init(
      int argc,
      FILE *ofp
      )
// STOP FUNC DECL
  {
    int status = 0;
  g_use_ram_dir = false;
  char *cptr = getenv("Q_USE_RAM_DIR");
  if ( ( cptr != NULL ) && ( strcasecmp(cptr, "true") == 0 ) ) {
    g_use_ram_dir = true;
  }

  zero_string(g_docroot,    (MAX_LEN_DIR_NAME+1));
  zero_string(g_gpu_server, (MAX_LEN_SERVER_NAME+1));
  g_gpu_port = -1;
  if ( ofp == NULL ) { 
    g_ofp = stdout;
  }
  else {
    g_ofp = ofp;
  }

#ifdef IPP
  ippInit();			//Automatic best static dispatch
#endif
  /*
    IPP initialization. This stage is required to take advantage of full
    IPP optimization. If Intel IPP program runs without ippInit(), by
    default the least optimized implementation is chosen. With ippInit()
    the best optimization layer will be dispatched at runtime. In
    certain debugging scenarios, it is helpful to force a specific
    implementation layer by using ippInitCpu, instead of the best as
    chosen by the dispatcher.
  */
#ifdef MAC_OSX
  g_num_cores = 1; 
#else
  g_num_cores = get_nprocs();
#endif
  if ( g_num_cores < 1 ) { go_BYE(-1); }
  zero_string(g_cwd, (MAX_LEN_DIR_NAME+1));
  if ( getcwd(g_cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); } 
  if ( ( argc < 2 ) || ( argc > MAX_NUM_ARGS ) ) { go_BYE(-1); }
BYE:
  return(status);
}
