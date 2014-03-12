    switch ( fldtype ) { 
    case I1 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i1val[i] __OP__ ll_minval ) { ll_minval = i1val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I2 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i2val[i] __OP__ ll_minval ) { ll_minval = i2val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i4val[i] __OP__ ll_minval ) { ll_minval = i4val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case I8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( i8val[i] __OP__ ll_minval ) { ll_minval = i8val[i]; }
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_minval, cum_nn_cnt);
      break;
    case F4 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( f4val[i] __OP__ dd_minval ) { dd_minval = f4val[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_minval, cum_nn_cnt);
      break;
    case F8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  if ( f8val[i] __OP__ dd_minval ) { dd_minval = f8val[i]; }
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_minval, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
