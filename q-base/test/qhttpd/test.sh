#!/usr/local/bin/bash
set -e 
server=localhost
port=8000
# Create one table space 
iter=1
while [ $iter -le 5 ]; do 
  rslt=`curl -s --url $server:$port/init?TABLESPACE=test1`
  id=`echo $rslt | cut -f 2 -d ":" | sed s'/ "//'g | sed s'/".*//'g`
  ids[$iter]=$id
  iter=`expr $iter + 1 `
  echo "ID $iter = $id"
done
echo "Array values ${ids[@]}"  # For displaying array elements
rslt=`curl -s --url $server:$port/init?TABLESPACE=test1`
echo rslt = $rslt | grep "Server busy"

iter=1
while [ $iter -le 5 ]; do 
  echo "${ids[$iter]}"
  iter=`expr $iter + 1 `
done
# let us work with tablespace 1 
iter=1
token=`echo "${ids[$iter]}"`

echo "==============="
ps aux | grep qhttpd
echo "==============="
instr="q f1s1opf2 t1 f1 10 '+' f2"
outstr=`qtils urlencode "$instr"`
command="$server:$port/q?TOKEN=$token&COMMAND=$outstr"
echo "command=$command"
rslt=`curl -s --url $command `
echo "Completed $instr. Result = $rslt"

echo "==============="
ps aux | grep qhttpd
echo "==============="
instr="q describe t1"
outstr=`qtils urlencode "$instr"`
command="$server:$port/q?TOKEN=$token&COMMAND=$outstr"
rslt=`curl -s --url $command `
echo "Completed $instr. Result = $rslt"

echo "==============="
ps aux | grep qhttpd
echo "==============="
instr="q is_fld t1 f1"
outstr=`qtils urlencode "$instr"`
command="$server:$port/q?TOKEN=$token&COMMAND=$outstr"
rslt=`curl -s --url $command `
echo "Completed $instr. Result = $rslt"

echo "==============="
ps aux | grep qhttpd
echo "==============="
command="$server:$port/shutdown"
rslt=`curl -s --url $command `
echo "Completed $instr. Result = $rslt"
