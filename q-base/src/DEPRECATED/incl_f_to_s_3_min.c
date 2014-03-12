switch ( fldtype ) { 
 case I1 : 
 case I2 : 
 case I4 : 
 case I8 : 
   ll_val = strtoll(valbuf, &endptr, 10);
   if ( *endptr != '\0' ) { go_BYE(-1); }
   if ( ll_val < ll_rslt ) { ll_rslt = ll_val; }
   break;
 case F4 : 
 case F8 : 
   dd_val = strtod(valbuf, &endptr);
   if ( *endptr != '\0' ) { go_BYE(-1); }
   if ( dd_val < dd_rslt ) { dd_rslt = dd_val; }
   break;
 default : 
   go_BYE(-1);
   break;
 }
