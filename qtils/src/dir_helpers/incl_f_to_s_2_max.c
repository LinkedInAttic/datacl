    switch ( fldtype ) { 
    case I1 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i1val[i] > ll_maxval ) { ll_maxval = i1val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I2 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i2val[i] > ll_maxval ) { ll_maxval = i2val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i4val[i] > ll_maxval ) { ll_maxval = i4val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case I8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i8val[i] > ll_maxval ) { ll_maxval = i8val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_maxval, cum_nn_cnt);
      break;
    case F4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( f4val[i] > dd_maxval ) { dd_maxval = f4val[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_maxval, cum_nn_cnt);
      break;
    case F8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( f8val[i] > dd_maxval ) { dd_maxval = f8val[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_maxval, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
