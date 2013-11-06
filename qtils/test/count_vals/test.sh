#/usr/local/bin/bash
set -e 
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
export Q_DOCROOT=$PWD/QD/
export Q_DATA_DIR=$PWD/QD/
rm -r -f $Q_DATA_DIR
mkdir    $Q_DATA_DIR
q init
#--------------------------------------------

nR=1000
q delete T1
q add_tbl T1 $nR
q s_to_f T1 fsrc "op=[seq]:start=[-$nR]:incr=[1]:fldtype=[I4]"

q delete T2
q add_tbl T2 $nR
q s_to_f T2 fsrc "op=[seq]:start=[-$nR]:incr=[2]:fldtype=[I4]"

q delete T3
q add_tbl T3 $nR
q s_to_f T3 fsrc "op=[seq]:start=[-$nR]:incr=[3]:fldtype=[I4]"

q rename T3 Tsrc
q app_tbl Tsrc T1
q app_tbl Tsrc T2

q dup_fld Tsrc fsrc srt_fsrc
q fop Tsrc srt_fsrc sortA
q count_vals Tsrc srt_fsrc '' Tdst val cnt
q pr_fld Tdst val:cnt '' _2

# cleanup
rm -f _*
rm -r -f $Q_DATA_DIR
echo "Successfully completed $0 in $PWD"
