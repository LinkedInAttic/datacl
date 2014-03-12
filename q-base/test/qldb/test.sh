#/usr/local/bin/bash
set -e 
prefix=_xxx

rm -r -f $PWD/ldb/
mkdir    $PWD/ldb
test -d  $PWD/ldb

qldb in1.txt $PWD/ldb/ ldbtest write  -1 1 $prefix
qldb in2.txt $PWD/ldb/ ldbtest append -1 1 $prefix

od -v  -i --width=4  $prefix.idx
od -v  -i --width=4  $prefix.len
od -v  -i --width=4  $prefix.off
od -v  -c --width=16 $prefix.vstr
od -v  -i --width=4  $prefix.I4
echo "========================================================"
qldb in1.txt $PWD/ldb/ ldbtest write  16 1 $prefix
qldb in2.txt $PWD/ldb/ ldbtest append 16 1 $prefix
od -v  -c --width=16 $prefix.cstr
od -v  -i --width=4 $prefix.I4
echo "========================================================"

# rm -f ${prefix}*
# rm -r -f $PWD/ldb/
