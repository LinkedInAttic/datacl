cilkfor ( int tid = 0; tid < nT; tid++ ) {
    if ( nn_X == NULL ) { 
      long long lb = tlb[tid];
      long long ub = tub[tid];
      nn_cnt[tid] = ub - lb; // all values are defined 
      switch ( fldtype ) {
      case I1 : 
	i1val[tid] = SCHAR_MIN;
	f_to_s_max_I1((char *)X, lb, ub, &(i1val[tid])); 
	cBYE(status);
	break;
      case I2 : 
	i2val[tid] = SHRT_MIN;
	f_to_s_max_I2((short *)X, lb, ub, &(i2val[tid])); 
	cBYE(status);
	break;
      case I4 : 
	i4val[tid] = INT_MIN;
	f_to_s_max_I4((int *)X, lb, ub, &(i4val[tid])); 
	cBYE(status);
	break;
      case I8 : 
	i8val[tid] = LLONG_MIN;
	f_to_s_max_I8((long long *)X, lb, ub, &(i8val[tid])); 
	cBYE(status);
	break;
      case F4 : 
	f4val[tid] = FLT_MIN;
	f_to_s_max_F4((float *)X, lb, ub, &(f4val[tid])); 
	cBYE(status);
	break;
      case F8 : 
	f8val[tid] = DBL_MIN;
	f_to_s_max_F8((double *)X, lb, ub, &(f8val[tid])); 
	cBYE(status);
	break;
      default : 
#ifndef ICC
	go_BYE(-1);
#endif
	break;
      }
    }
    else {
      nn_cnt[tid] = 0; // no idea how many values are defined 
      switch ( fldtype ) {
      case I1 : 
	nn_f_to_s_max_I1((char *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(i1val[tid])); 
	cBYE(status);
	break;
      case I2 : 
	nn_f_to_s_max_I2((short *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(i2val[tid])); 
	cBYE(status);
	break;
      case I4 : 
	nn_f_to_s_max_I4((int *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(i4val[tid])); 
	cBYE(status);
	break;
      case I8 : 
	nn_f_to_s_max_I8((long long *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(i8val[tid])); 
	cBYE(status);
	break;
      case F4 : 
	nn_f_to_s_max_F4((float *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(f4val[tid])); 
	cBYE(status);
	break;
      case F8 : 
	nn_f_to_s_max_F8((double *)X, nn_X, lb, ub, 
			 &(nn_cnt[tid]), &(f8val[tid])); 
	cBYE(status);
	break;
      default : 
#ifndef ICC
	go_BYE(-1);
#endif
	break;
      }
    }
}
