/* DO NOT AUTO GENERATE HEADER FILE  */
// XTYPE I4
// ITYPE int
// NBITS 32
      __XTYPE__ptr = (__ITYPE__*)X; __XTYPE__ptr += lb;
#ifdef IPP
      ippsSet___NBITS__s(__XTYPE__val, __XTYPE__ptr, nX);
#else
      assign_const___XTYPE__(__XTYPE__ptr, nX, __XTYPE__val);
#endif
      break;
