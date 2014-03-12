#/usr/local/bin/bash
# shopt -s expand_aliases
# alias q='valgrind $valgrind_options q'
set -e
export Q_DOCROOT=$PWD/QD/
export Q_DSK_DATA_DIR=$PWD/QD/
export Q_RAM_DATA_DIR=$PWD/QD/
export Q_RUN_TIME_CHECKS=TRUE
rm -r -f          $PWD/QD/
mkdir             $PWD/QD/
q init $Q_DSK_DATA_DIR ""
q dld TM  TM.meta.csv  TM.csv  'ignore_hdr=[false]'
q dld TC  TC.meta.csv  TC.csv  'ignore_hdr=[false]'
q dld TD1 TD1.meta.csv TD1.csv 'ignore_hdr=[false]'
q udf_deg2 TM TC_lb TC_cnt TD1 fk_TM TC mid TD2 mid
q pr_fld TD2 mid 
