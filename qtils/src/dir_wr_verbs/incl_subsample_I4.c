    I4f1 = (int *)f1_X; I4f1 += lb1; // for access to source
    I4f2 = (int *)f2_X; I4f2 += lb2; // for access to destination

    for ( int i = 0; i < range2; i++ ) {
	for ( bool found = false; found == false; ) { 
	  rnum = mix_I4(rnum);
	  int x = rnum % ratio;
	  if ( x == 0 ) {
	    *I4f2++ = *I4f1;
	    chk_nR2++;
	    found = true;
	  }
	  I4f1++;
	  if ( I4f1 >= max_I4f1 ) {
	    I4f1 = (int *)f1_X; I4f1 += lb1; // for access to source
	  }
	}
      }
