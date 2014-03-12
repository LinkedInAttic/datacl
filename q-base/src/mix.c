/* DO NOT AUTO GENERATE HEADER FILE  */
unsigned int 
mix_I4( 
    unsigned int a
    )
{
  a = (a+0x7ed55d16) + (a<<12);
  a = (a^0xc761c23c) ^ (a>>19);
  a = (a+0x165667b1) + (a<<5);
  a = (a+0xd3a2646c) ^ (a<<9);
  a = (a+0xfd7046c5) + (a<<3);
  a = (a^0xb55a4f09) ^ (a>>16);
  return a;
}

unsigned int
mix_I8(
    unsigned long long a
    )
{
  unsigned long long ultemp;
  ultemp = a >> 32;
  unsigned int i1 = (unsigned int)ultemp;
  ultemp = ( a << 32 ) >> 32;
  unsigned int i2 = (unsigned int)ultemp;
  i1 = mix_I4(i1);
  i2 = mix_I4(i2);
  unsigned int i3 = i1^i2;
  return i3;
} 

