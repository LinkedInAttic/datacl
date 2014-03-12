#!usr/local/bin/bash
set -e 
x=`cat 2.json`
y=`qtils urlencode "$x"`
iter=1
rm -f /tmp/results.txt
tstart=` qtils time_since_epoch usec`
concurrent=10
niters=5
while [ $iter -le $niters ]; do 
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt & 
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt  &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  curl -s localhost:8080/plato/plato2.php?QUERY=$y  >> /tmp/results.txt &
  wait
  echo "iter = $iter"
  iter=`expr $iter + 1`
done
total_iters=`echo "$iter * $concurrent" | bc`
tstop=` qtils time_since_epoch usec`
t=`echo "(($tstop - $tstart) / 1000.0) / 10 " | bc`
echo "Number of iterations = $total_iters"
echo "Concurrent users     = $concurrent"
echo "Average time in msec = $t"
# echo "tstart               = $tstart"
# echo "tstop                = $tstop "
