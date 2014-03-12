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

ylc init 
ylc add xxx TB
set +e
ylc add xxx TB
set -e
ylc add yyy TB
ylc list_lists
ylc push xxx bis_recruiter
ylc push xxx bis_hr
ylc describe xxx 
set +e
ylc push xxx def
set -e
ylc pop xxx
ylc pop xxx
set +e
ylc pop xxx
ylc pop yyy
set -e
ylc del yyy
set +e
ylc del zzz
set -e
ylc list_lists
ylc push xxx bis_recruiter
ylc push xxx bis_hr
ylc push xxx b1
ylc push xxx b2
ylc describe xxx
ylc limit xxx 10000:20000:-1:100000000
ylc describe xxx
ylc limit xxx 200000000:-1:10:-1
ylc describe xxx
ylc check
chk_old_fields
echo PREMATURE; exit 1; 
#----------
echo "--- TESTING EXCLUSIONS ------------------------------------"
ylc describe  xxx exclusions
ylc exclude   xxx bis_hr
chk_old_fields
ylc check
ylc exclude   xxx bis_hr
ylc exclude   xxx bis_recruiter
ylc describe  xxx exclusions
ylc unexclude xxx "" 
ylc check ; chk_old_fields;
ylc exclude   xxx bis_hr
ylc exclude   xxx bis_hr
ylc check; chk_old_fields
ylc unexclude xxx bis_hr
ylc describe  xxx exclusions
ylc unexclude xxx bis_recruiter
ylc describe  xxx exclusions

ylc exclude   xxx bis_hr
ylc exclude   xxx bis_recruiter
ylc exclude   xxx b3
ylc exclude   xxx b4
ylc exclude   xxx b5
ylc unexclude xxx b3
ylc unexclude xxx b4
ylc unexclude xxx b5
ylc unexclude xxx "" 
ylc pop xxx
ylc pop xxx
ylc pop xxx
ylc pop xxx
ylc pop xxx
ylc describe xxx
ylc check ; chk_old_fields
echo ALL_DONE; exit 1; 
