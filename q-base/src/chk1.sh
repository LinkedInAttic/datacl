# Making sure old fields still exist
function foo()
{
    fld=$1
        isfld=`q is_fld TB $fld | cut -f 1 -d ","`
	  if [ $isfld = 0 ]; then echo "Field $fld missing"; exit 1; fi
}
function chk_old_fields()
{
  foo b_is_hr
    foo b_is_recruiter
    foo b1
    foo b2
    foo b3
    echo "old fields exist";
}

chk_old_fields
