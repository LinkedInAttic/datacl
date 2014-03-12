    I8f1 = (long long *)f1_X; I8f1 += lb1; // for access to source
    I8f2 = (long long *)f2_X; I8f2 += lb2; // for access to destination

    for ( int i = 0; i < range2; i++ ) {
	for ( bool found = false; found == false; ) { 
	  rnum = mix_I8(rnum);
	  int x = rnum % ratio;
	  if ( x == 0 ) {
	    *I8f2++ = *I8f1;
	    chk_nR2++;
	    found = true;
	  }
	  I8f1++;
	  if ( I8f1 >= max_I8f1 ) {
	    I8f1 = (long long *)f1_X; I8f1 += lb1; // for access to source
	  }
	}
      }
