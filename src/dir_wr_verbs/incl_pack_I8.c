    for ( long long i = 0; i < nR; i++ ) {
      long long *fout = (long long *)fout_X;
      long long outval = 0;
      for ( int j = 0; j < n_flds; j++ ) { 
	long long thisval = 0;
	char      *I1ptr = (char      *)Xs[j];
	short     *I2ptr = (short     *)Xs[j];
	int       *I4ptr = (int       *)Xs[j];
	long long *I8ptr = (long long *)Xs[j];
	switch ( fld_recs[j].fldtype ) {
	case I1 : thisval = I1ptr[i]; break; 
	case I2 : thisval = I2ptr[i]; break; 
	case I4 : thisval = I4ptr[i]; break; 
	case I8 : thisval = I8ptr[i]; break; 
	default : break;
	}
	thisval = thisval << shifts[j];
	outval = outval | thisval;
      }
      fout[i] = outval;
    }
