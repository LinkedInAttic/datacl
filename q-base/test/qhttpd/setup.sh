#!/usr/local/bin/bash
# Create one table space 
export Q_DOCROOT=$PWD/QD1
export Q_DATA_DIR=$PWD/QD1
rm -f valgrind.log
rm -r -f QD* # This is to wipe out temporary session directories
pkill qhttpd
pkill valgrind
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
q add_tbl t1 10
q mk_idx t1 f1 I4
# Create another table space 
export Q_DOCROOT=$PWD/QD2
export Q_DATA_DIR=$PWD/QD2
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
q add_tbl t2 10
q mk_idx t2 f2 I4
#---------------------
unset Q_DOCROOT
unset Q_DATA_DIR
#---------------------
# Create conf file 
rm -f qhttpd.conf
echo "lgcl_name,q_docroot,q_data_dir,bs_docroot" >> qhttpd.conf
iter=1
while [ $iter -le 5 ]; do 
  echo "test1,$PWD/QD1${iter}/,$PWD/QD1${iter},$PWD/QD1" >> qhttpd.conf
  iter=`expr $iter + 1 `
done
echo PREMATURE; exit 1; 
valgrind --log-file=valgrind.log --leak-check=full --show-reachable=yes --track-origins=yes qhttpd qhttpd.conf 
sleep 1
echo "Started qhtttpd"
