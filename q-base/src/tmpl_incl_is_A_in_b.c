/* DO NOT AUTO GENERATE HEADER FILE  */
      for  ( long long i1 = 0; i1 < nX; i1++ ) {
	__ITYPE__       f1_val___XTYPE__ = f1___XTYPE__[i1];
	bool found = false;
	if ( l_f2___XTYPE__ < max_f2___XTYPE__ ) { 
	  assign_const_I1(l_cfX+i1, nX-i1, 0);
	  break;
	}
	for ( ; l_f2___XTYPE__ < max_f2___XTYPE__; ) { 
	  __ITYPE__ f2_val___XTYPE__ = *l_f2___XTYPE__;
	  if ( f2_val___XTYPE__ < f1_val___XTYPE__ ) {
	    l_f2___XTYPE__++;
	    continue;
	  }
	  else if ( f2_val___XTYPE__ == f1_val___XTYPE__ ) {
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
