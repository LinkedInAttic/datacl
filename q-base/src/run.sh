#!/bin/sh
LOC=$HOME/WORK/TITLE/src
DB=db1.sq3
VG="valgrind --leak-check=full --track-fds=yes --show-reachable=yes --track-origins=yes"
VG=""
export Q_DOCROOT=$PWD/$DB

#--cleanup
$VG q del_fld "" member c2
$VG q del_tbl "" sel_member
$VG q del_tbl "" uq_sel_member
$VG q del_tbl "" whash
$VG q del_tbl "" word_histo
$VG q del_fld "" member c1
#-- start
$VG q f1s1opf2 "" member twhash "603682462:552105066:649187980" c1
$VG q copy_fld "" member mid c1 sel_member
#-- $VG q pr_fld "" sel_member mid "" "" "" 
$VG q f1op "" sel_member mid sortA
$VG q f1s1opf2 "" sel_member mid is_prev_same c1
#-- $VG q pr_fld "" sel_member c1 "" "" "" 
$VG q copy_fld "" sel_member mid c1 uq_sel_member
#-- $VG q pr_fld "" uq_sel_member mid "" 
$VG q is_a_in_b "" member mid uq_sel_member mid c2
#-- $VG q pr_fld "" member c2 "" "" "" 
$VG q f1f2op "" member twhash c2 sel_word whash flatten
#-- $VG q pr_fld "" sel_word whash "" "" "" 
$VG q f1op "" sel_word whash sortA;
#-- $VG q pr_fld "" sel_word whash "" "" "" 
$VG q f1f2op "" sel_word whash "" word_histo whash histo
#-- $VG q pr_fld "" word_histo whash "" "" "" 
#-- $VG q pr_fld "" word_histo cnt "" "" "" 
$VG q join "" twdict twhash tword word_histo whash tword
$CVG q dup_fld "" word_histo cnt srt_cnt
$VG q f1op "" word_histo srt_cnt sortD
$VG q pr_fld "" word_histo cnt "" ""  _cnt
$VG q pr_fld "" word_histo tword "" "" _tword
php glue.php _word_histo.csv '"' '"' , "" eoln _cnt  _tword
