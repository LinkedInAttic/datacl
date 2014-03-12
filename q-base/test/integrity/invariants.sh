#/usr/local/bin/bash
shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD
export Q_DATA_DIR=/tmp/
#--------------------------------------------
# Table names cannot be null
q f1s1opf2 tbl _sz_name 1 '<' x
n=`q f_to_s tbl x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------
# number of rows must be >= 1  
q f1s1opf2 tbl nR 1 '<' x
n=`q f_to_s tbl x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------
# Table names must be unique
q count_strings tbl name 1024 tempt 
q f1s1opf2 tempt cnt 1 '>' x
n=`q f_to_s tempt x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------
# Non-null dispnames must be unique
q count_strings tbl dispname 1024 tempt 
q f1s1opf2 tempt cnt 1 '>' x
q f1f2opf3 tempt _nn_dispname x '&&' y
n=`q f_to_s tempt y sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------
#--------------------------------------------
# A table must have at least one field 
q s_to_f fld one 'op=const:val=1:fldtype=int'
q mjoin fld tbl_id one tbl tbl_id num_flds sum
q f1s1opf2 tbl num_flds 1 '<' x
n=`q f_to_s tbl x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# Field "tbl_id" in Table "fld" is fk to field "tbl_id" in Table 'tbl"
q dup_fld tbl tbl_id srt_tbl_id
q fop tbl srt_tbl_id sortA
q is_a_in_b fld tbl_id tbl srt_tbl_id x '' ''
q f1opf2 fld x 'op=!' y
n=`q f_to_s fld y sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# Filenames must be unique 
q count_strings fld filename 1024 tempt
q f1s1opf2 tempt cnt 1 '>' x
n=`q f_to_s tempt x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------
# parent_id < 0 <=> auxtype < 0 
q f1s1opf2 fld parent_id 0 '<' x 
q f1s1opf2 fld auxtype   0 '>=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# nn_fld_id >= 0 => must point to legit field
q f1s1opf2 fld parent_id 0 '<' x
n=`q f_to_s fld x sum`
q del_tbl t2
q add_tbl t2 $n
q copy_fld fld fld_id x t2

q f1s1opf2 fld nn_fld_id 0 '>=' x
n=`q f_to_s fld x sum`
if [ $n -ge 0 ]; then 
  q del_tbl t1
  q add_tbl t1 $n
  q copy_fld fld nn_fld_id x t1
  q copy_fld fld    fld_id x t1
  q sortf1f2 t1 nn_fld_id fld_id A_

  q is_a_in_b t1 nn_fld_id t2 fld_id x '' ''
  chk_n=`q f_to_s t1 x sum`
  if [ $n != $chk_n ]; then 
    q f1opf2 t1 x 'op=!' y
    echo FAILURE: Printing some bad guys;
    q pr_flds t1  fld_id:nn_fld_id | head 
  fi
fi

# nn_fld_id >= 0 => parent_id < 0
q f1s1opf2 fld nn_fld_id 0 '>=' x 
q f1s1opf2 fld parent_id 0 '>=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# sz_fld_id >= 0 => parent_id < 0
q f1s1opf2 fld sz_fld_id 0 '>=' x 
q f1s1opf2 fld parent_id 0 '>=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# fldtype = "char string" => sz_fld_id >= 0 
q f1s1opf2 fld fldtype 'char string' '==' x 
q f1s1opf2 fld sz_fld_id 0 '<' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# fldtype = "long long" => n_sizeof  = 8
q f1s1opf2 fld fldtype 'long long' '==' x 
q f1s1opf2 fld n_sizeof 8 '!=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# fldtype = "int" => n_sizeof  = 4
q f1s1opf2 fld fldtype 'int' '==' x 
q f1s1opf2 fld n_sizeof 4 '!=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# fldtype = "bool" => n_sizeof  = 1
q f1s1opf2 fld fldtype 'bool' '==' x 
q f1s1opf2 fld n_sizeof 1 '!=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# fldtype = "char" => n_sizeof  = 1
q f1s1opf2 fld fldtype 'char' '==' x 
q f1s1opf2 fld n_sizeof 1 '!=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

q f1opf2 tbl tbl_id 'op=conv:newtype=long long' l_tbl_id
q f1opf2 fld tbl_id 'op=conv:newtype=long long' l_tbl_id
q f1opf2 fld n_sizeof 'op=conv:newtype=long long' l_n_sizeof
q mjoin tbl l_tbl_id nR fld l_tbl_id nR reg
q f1f2opf3 fld l_n_sizeof nR '*' chk_filesize

# n_sizeof > 0 => fldsize = (n_sizeof * nR)
q f1s1opf2 fld n_sizeof 0 '>' x 
q f1f2opf3 fld filesize chk_filesize '!=' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# n_sizeof = 0 => fldsize >= nR
q f1s1opf2 fld n_sizeof 0 '==' x 
q f1f2opf3 fld filesize nR '<' y
q f1f2opf3 fld x y '&&' z
n=`q f_to_s fld z sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

#---------------------------------------
# No two rows can have same nn_fld_id 
q f1s1opf2 fld nn_fld_id 0 '>=' x
n=`q f_to_s fld x sum`
if [ $n = 0 ]; then echo STRANGE: $LINENO; exit 1; fi 
q del_tbl t1
q add_tbl t1 $n
q copy_fld fld nn_fld_id x t1
q fop t1 nn_fld_id sortA
q count_vals t1 nn_fld_id '' t2 nn_fld_id cnt
q f1s1opf2 t2 cnt 1 '>' x 
n=`q f_to_s t2 x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# No two rows can have same sz_fld_id 
q f1s1opf2 fld sz_fld_id 0 '>=' x
n=`q f_to_s fld x sum`
if [ $n = 0 ]; then echo STRANGE: $LINENO; exit 1; fi 
q del_tbl t1
q add_tbl t1 $n
q copy_fld fld sz_fld_id x t1
q fop t1 sz_fld_id sortA
q count_vals t1 sz_fld_id '' t2 sz_fld_id cnt
q f1s1opf2 t2 cnt 1 '>' x 
n=`q f_to_s t2 x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# Number of rows with same parent_id = 0, 1, 2 but not greater
q s_to_f fld one 'op=const:val=1:fldtype=int'
q mjoin fld parent_id one fld fld_id num_children sum
q f1s1opf2 fld num_children 2 '>' x
n=`q f_to_s fld x sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# sorttype \in \{ -1, 0, 1, 2 \}
q f1s1opf2 fld sorttype "-1:0:1:2" '==' x 
q f1opf2   fld x 'op=!' y
n=`q f_to_s fld y sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 

# Non-null dispnames must be unique
q count_strings fld dispname 1024 tempt 
q f1s1opf2 tempt cnt 1 '>' x
q f1f2opf3 tempt _nn_dispname x '&&' y
n=`q f_to_s tempt y sum`
if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
#--------------------------------------------

# g_fld[fld_id].nn_fld_id >= 0 => g_fld[nn_fld_id].parent_id = fld_id
q f1s1opf2 fld nn_fld_id 0 '>=' x 
n=`q f_to_s fld x sum`
if [ $n = 0 ]; then
  echo "Skipping this test: $LINENO"; 
else
  q del_tbl t1
  q add_tbl t1 $n
  q copy_fld fld parent_id x t1
  q copy_fld fld    fld_id x t1
  q mjoin fld fld_id nn_fld_id t1 parent_id chk_nn_fld_id reg
  q f1f2opf3 t1 fld_id chk_nn_fld_id '!=' x 
  n=`q f_to_s t1 x sum`
  if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
fi
# g_fld[fld_id].sz_fld_id >= 0 => g_fld[sz_fld_id].parent_id = fld_id
q f1s1opf2 fld sz_fld_id 0 '>=' x 
n=`q f_to_s fld x sum`
if [ $n = 0 ]; then
  echo "Skipping this test: $LINENO"; 
else
  q del_tbl t1
  q add_tbl t1 $n
  q copy_fld fld parent_id x t1
  q copy_fld fld    fld_id x t1
  q mjoin fld fld_id sz_fld_id t1 parent_id chk_sz_fld_id reg
  q f1f2opf3 t1 fld_id chk_sz_fld_id '!=' x 
  n=`q f_to_s t1 x sum`
  if [ $n != 0 ]; then echo FAILURE: $LINENO; exit 1; fi 
fi

echo ALL INVARIANTS DONE
