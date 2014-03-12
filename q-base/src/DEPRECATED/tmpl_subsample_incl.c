    __XTYPE__f1 = (__ITYPE__ *)f1_X; __XTYPE__f1 += lb1; // for access to source
    __XTYPE__f2 = (__ITYPE__ *)f2_X; __XTYPE__f2 += lb2; // for access to destination

    for ( int i = 0; i < range2; i++ ) {
	for ( bool found = false; found == false; ) { 
	  rnum = mix___XTYPE__(rnum);
	  int x = rnum % ratio;
	  if ( x == 0 ) {
	    *__XTYPE__f2++ = *__XTYPE__f1;
	    chk_nR2++;
	    found = true;
	  }
	  __XTYPE__f1++;
	  if ( __XTYPE__f1 >= max___XTYPE__f1 ) {
	    __XTYPE__f1 = (__ITYPE__ *)f1_X; __XTYPE__f1 += lb1; // for access to source
	  }
	}
      }
