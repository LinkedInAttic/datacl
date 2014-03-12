/* DO NOT AUTO GENERATE HEADER FILE  */
      for  ( long long i1 = 0; i1 < nX; i1++ ) {
	int       f1_val_I8 = f1_I8[i1];
	bool found = false;
	if ( l_f2_I8 < max_f2_I8 ) { 
	  assign_const_I1(l_cfX+i1, nX-i1, 0);
	  break;
	}
	for ( ; l_f2_I8 < max_f2_I8; ) { 
	  int f2_val_I8 = *l_f2_I8;
	  if ( f2_val_I8 < f1_val_I8 ) {
	    l_f2_I8++;
	    continue;
	  }
	  else if ( f2_val_I8 == f1_val_I8 ) {
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
