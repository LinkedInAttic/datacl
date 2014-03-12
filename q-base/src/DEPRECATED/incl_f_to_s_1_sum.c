for ( int tid = 0; tid < nT; tid++ ) {
    if ( nn_X == NULL ) { 
      lb = tlb[tid];
      ub = tub[tid];
      nn_cnt[tid] = ub - lb; // all values are defined 
      switch ( fldtype ) {
      case I1 : 
	f_to_s_sum_I1((char *)X, lb, ub, &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I2 : 
	f_to_s_sum_I2((short *)X, lb, ub, &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I4 : 
	f_to_s_sum_I4((int *)X, lb, ub, &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I8 : 
	f_to_s_sum_I8((long long *)X, lb, ub, &(ll_numer[tid])); 
	cBYE(status);
	break;
      case F4 : 
	f4val[tid] = FLT_MAX;
	f_to_s_sum_F4((float *)X, lb, ub, &(dd_numer[tid])); 
	cBYE(status);
	break;
      case F8 : 
	f8val[tid] = DBL_MAX;
	f_to_s_sum_F8((double *)X, lb, ub, &(dd_numer[tid])); 
	cBYE(status);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
    else {
      nn_cnt[tid] = 0; // no idea how many values are defined 
      switch ( fldtype ) {
      case I1 : 
	nn_f_to_s_sum_I1((char *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I2 : 
	nn_f_to_s_sum_I2((short *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I4 : 
	nn_f_to_s_sum_I4((int *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(ll_numer[tid])); 
	cBYE(status);
	break;
      case I8 : 
	nn_f_to_s_sum_I8((long long *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(ll_numer[tid])); 
	cBYE(status);
	break;
      case F4 : 
	nn_f_to_s_sum_F4((float *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(dd_numer[tid])); 
	cBYE(status);
	break;
      case F8 : 
	nn_f_to_s_sum_F8((double *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(dd_numer[tid])); 
	cBYE(status);
	break;
      default : 
	go_BYE(-1);
	break;
      }
    }
}
