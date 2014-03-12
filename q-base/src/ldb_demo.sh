set -e 
export Q_DOCROOT=$PWD/SIMON/
export Q_DATA_DIR=$PWD/SIMON/
rm -r -f $Q_DOCROOT; 
mkdir $Q_DOCROOT
q init
q dld Tsimon simon.meta.csv jowu_dump_countries ''
#---------------------------------------------------
q dup_fld Tsimon country lcountry
q set_meta Tsimon lcountry fldtype I8
q describe Tsimon
q pr_fld Tsimon country:lcountry | head -20
#---------------------------------------------------
q count_ht Tsimon lcountry Thisto
q bindmp Thisto count:value '' _tempf
sortbindmp _tempf LL DD
q binld Thisto count:value   I8:I8 _tempf
q pr_fld Thisto value:count | head 

