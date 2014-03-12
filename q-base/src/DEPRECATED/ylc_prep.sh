#!/usr/local/bin/bash
set -e 
# shopt -s expand_aliases
# alias ylc="valgrind $valgrind_options ylc"

# Making sure old fields still exist
function foo()
{
  fld=$1
  isfld=`q is_fld TB $fld | cut -f 1 -d ","`
  if [ $isfld = 0 ]; then echo "Field $fld missing"; exit 1; fi 
}
function chk_old_fields()
{
foo bis_hr
foo bis_recruiter
foo b1
foo b2
foo b3
echo "old fields exist";
}
# Make some temporary "segments"
q f1opf2 TB is_hr 'op=[conv]:newtype=[B]' bis_hr
q f1opf2 TB is_recruiter 'op=[conv]:newtype=[B]' bis_recruiter
q f1s1opf2 TB connections 90:100 '>=&&<=' x
q f1opf2 TB x 'op=[conv]:newtype=[B]' b1
q f1s1opf2 TB connections 89:90 '>=&&<=' x
q f1opf2 TB x 'op=[conv]:newtype=[B]' b2
q f1s1opf2 TB connections 99:100 '>=&&<=' x
q f1opf2 TB x 'op=[conv]:newtype=[B]' b3
q f1s1opf2 TB connections 98:99 '>=&&<=' x
q f1opf2 TB x 'op=[conv]:newtype=[B]' b4
q f1s1opf2 TB connections 97:98 '>=&&<=' x
q f1opf2 TB x 'op=[conv]:newtype=[B]' b5
q delete TB x 

