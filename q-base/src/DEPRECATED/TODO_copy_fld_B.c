      if ( nnX == NULL ) {
#ifdef XXXX
	cfld_I8 = (unsigned long long *) cfld_X;
	nR1_I8 = nR1 / NUM_BITS_IN_I8;
	if ( ( nR1_I8 * NUM_BITS_IN_I8 ) != nR1 ) { nR1_I8++; }
	for ( long long j = 0; j < nR1_I8; j++ ) {
	  // handle special case when next 64 bits all 0
	  if ( cfld_I8[j] == 0 ) { 
	    X   += (fldsz * NUM_BITS_IN_I8);
	    continue; 
	  }
	  // handle special case when next 64 bits all 1
	  if ( cfld_I8[j] == 0xFFFFFFFFFFFFFFFF ) { 
	    memcpy(opX, X, (fldsz * NUM_BITS_IN_I8));
	    X   += (fldsz * NUM_BITS_IN_I8);
	    opX += (fldsz * NUM_BITS_IN_I8);
	    continue; 
	  }
	  /* 8 bits to a byte, 8 bytes to a long long */
	  lb = j  * 8;
	  ub = lb + 8;
	  for ( long long i = lb; i < ub; i++ ) { 
	  }
	  word_idx = i >> 3; 
	  bit_idx  = i - ( word_idx << 3);
	  if ( cfldX[i] == TRUE ) { 
	    memcpy(opX, X, fldsz);
	    opX += fldsz;
	    dbg_nR2++;
	  }
	}
#endif
