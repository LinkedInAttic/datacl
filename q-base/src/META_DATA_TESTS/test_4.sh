#/usr/local/bin/bash
set -e 

##C \begin{invariant}
##C \label{inv_4}
##C If field is sorted
##C \begin{enumerate}
##C \item fldtype must be one of I1, I2, I4, I8, F4, F8. 
##C \item cannot have null values i.e., no {\tt nn} field 
##C \end{enumerate}
##C \end{invariant}

function foo_srt()
{
  fldtype=$1
  outfld=$2
  q regex_match lkp_fldtype txt $fldtype exact x
  idx=`q f_to_s lkp_fldtype x 'op=[get_idx]:val=[1]'`
  q f1s1opf2 Tflds fldtype $idx '==' tempf
  q f1f2opf3 Tflds tempf $outfld '||' $outfld
}


function foo_fld()
{
  fldtype=$1
  q regex_match lkp_fldtype txt $fldtype exact x
  idx=`q f_to_s lkp_fldtype x 'op=[get_idx]:val=[1]'`
  q f1s1opf2 Tflds fldtype $idx '==' tempf
  q f1f2opf3 Tflds tempf y '||' y
}

# Let x indicate fields that are sorted
q s_to_f Tflds x 'op=[const]:val=[0]:fldtype=[I1]'
foo_srt ascending x
foo_srt desceding x
foo_srt unsorted  x

# Let y indicate fields with type = one of I1, I2, I4, I8, F4, F8
q s_to_f Tflds y 'op=[const]:val=[0]:fldtype=[I1]'
foo_fld I1 y
foo_fld I2 y
foo_fld I4 y
foo_fld I8 y
foo_fld F4 y
foo_fld F8 y

# Let z = x and not y
q f1f2opf3 Tflds x y '&&!' z
n=`q f_to_s Tflds z sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then exit 1; fi 

# Let y = fields that have null values
q f1s1opf2 Tflds nn_fld_id 0 '>=' y
q f1f2opf3 Tflds x y '&&' z
n=`q f_to_s Tflds z sum | cut -f 1 -d ":"`
if [ $n != 0 ]; then exit 1; fi 


q delete Tflds x:y:z:tempf
echo "Completed $0 in $PWD"
