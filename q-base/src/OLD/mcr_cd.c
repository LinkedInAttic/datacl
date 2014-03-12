  char cwd[MAX_LEN_DIR_NAME+1];
  bool is_cd = false;
  extern char *g_data_dir;
  if ( getcwd(cwd, MAX_LEN_DIR_NAME) == NULL ) { go_BYE(-1); }
  if ( ( g_data_dir != NULL ) && ( *g_data_dir != '\0' ) ) { 
    chdir(g_data_dir);
    is_cd = true;
  }
