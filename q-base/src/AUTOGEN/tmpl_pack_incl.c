/* DO NOT AUTO GENERATE HEADER FILE  */
#pragma cilk grainsize = 4096
    cilkfor ( long long i = 0; i < nR; i++ ) {
      __ITYPE__ *fout = (__ITYPE__ *)fout_X;
      __ITYPE__ outval = 0;
      if ( status < 0 ) { continue; }
      for ( int j = 0; j < n_flds; j++ ) { 
	__ITYPE__ thisval = 0;
	switch ( fld_recs[j].fldtype ) {
	case I1 : 
	  I1ptr = (char      *)Xs[j]; 
	  thisval = I1ptr[i]; 
	  break; 
	case I2 : 
	  I2ptr = (short     *)Xs[j]; 
	  thisval = I2ptr[i]; 
	  break; 
	case I4 : 
	  I4ptr = (int       *)Xs[j]; 
	  thisval = I4ptr[i]; 
	  break; 
	case I8 : 
	  I8ptr = (long long *)Xs[j]; 
	  thisval = I8ptr[i]; 
	  break; 
	default : break;
	}
        if ( thisval < 0 ) { status = -1; continue; }
	thisval = thisval << shifts[j];
	outval = outval | thisval;
      }
      fout[i] = outval;
    }
