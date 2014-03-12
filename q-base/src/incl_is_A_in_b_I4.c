/* DO NOT AUTO GENERATE HEADER FILE  */
      for  ( long long i1 = 0; i1 < nX; i1++ ) {
	int       f1_val_I4 = f1_I4[i1];
	bool found = false;
	if ( l_f2_I4 < max_f2_I4 ) { 
	  assign_const_I1(l_cfX+i1, nX-i1, 0);
	  break;
	}
	for ( ; l_f2_I4 < max_f2_I4; ) { 
	  int f2_val_I4 = *l_f2_I4;
	  if ( f2_val_I4 < f1_val_I4 ) {
	    l_f2_I4++;
	    continue;
	  }
	  else if ( f2_val_I4 == f1_val_I4 ) {
	    found = true;
	    break;
	  }
	  else {
	    found = false;
	    break;
	  }
	}
	l_cfX[i1] = found;
      }
      break;
