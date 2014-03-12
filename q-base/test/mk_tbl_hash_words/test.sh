#!/bin/sh
set -e
VG="valgrind --leak-check=full --track-fds=yes --show-reachable=yes --track-origins=yes"
VG=""
DB=$PWD/db1.sq3
export VALGRIND="$VG"
sqlite3 db1.sq3 < ../../src/docroot.sql
export Q_DOCROOT=$PWD/db1.sq3

t1=t1
t2=t2

$VG q del_tbl r1
$VG q dld r1 region.meta.csv region.csv 1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1:1
$VG q mk_tbl_hash_words r1 description r2 r3 "len=8:break_on_ws=true:xform=1"
$VG q mk_tbl_hash_words r1 description r2 r3 "len=4:break_on_ws=true:xform=1"
$VG q mk_tbl_hash_words r1 description r2 r3 "len=8:break_on_ws=false:xform=1"
$VG q mk_tbl_hash_words r1 description r2 r3 "len=4:break_on_ws=false:xform=1"
#-- cleanup
# $VG q del_tbl r1
