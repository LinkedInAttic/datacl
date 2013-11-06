    switch ( fldtype ) { 
    case I1 : 
    case I2 : 
    case I4 : 
    case I8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  ll_sum += ll_numer[i];
	}
      }
      sprintf(rslt_buf, "%lld:%lld", ll_sum, cum_nn_cnt);
      break;
    case F4 : 
    case F8 : 
      for ( int i = 0; i < nT; i++ ) { 
	if ( nn_cnt[i] > 0 ) { 
	  cum_nn_cnt += nn_cnt[i];
	  dd_sum += dd_numer[i];
	}
      }
      sprintf(rslt_buf, "%lf:%lld", dd_sum, cum_nn_cnt);
      break;
    default : 
      go_BYE(-1);
      break;
    }
