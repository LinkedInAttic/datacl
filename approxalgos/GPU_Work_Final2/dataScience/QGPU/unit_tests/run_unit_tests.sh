#=============================================================

fn=f1s1opf2
cd f1s1opf2

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=f1f2opf3
cd f1f2opf3

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=f1f2_to_s
cd f1f2_to_s

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=f1_shift_f2
cd f1_shift_f2

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=count
cd count

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=count_f
cd count_f

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "failure on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "failure on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=join
cd join

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "failure on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=f1opf2
cd f1opf2

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=mk_idx
cd mk_idx

bash test1_I1.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=sortf1f2
cd sortf1f2

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=funnel
cd funnel

bash test1_I4.sh > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

fn=f_to_s
cd f_to_s

perl test.pl > /tmp/testout.dat
diff -b /tmp/testout.dat expected_output.dat
if [ $? != 0 ]; then echo "FAILURE on [$fn]"; exit 1; fi 

cd ..

#=============================================================

echo "UNIT TESTS SUCCESSFUL!!"

#=============================================================
