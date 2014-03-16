#compile data generator
cd datagen
make
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
cd ..


#I1 data files
./datagen/datagen_I1 data/1e5_incr_mod100_I1.bin 100000 incr_mod100 
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I1 data/1e5_allones_I1.bin 100000 allones
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I1 data/1e5_evenones_I1.bin 100000 evenones
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I1 data/1e5_all10s_I1.bin 100000 all10s
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I1 data/1e5_histdata_I1.bin 100000 rand-0-10
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 
./datagen/datagen_I4 data/1e5_incr_mod100_I4.bin 100000 incr_mod100 
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 



#I4 data files
./datagen/datagen_I4 data/1e5_allones_I4.bin 100000 allones
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1e5_evenones_I4.bin 100000 evenones
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1e5_all10s_I4.bin 100000 all10s
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1e5_histdata_I4.bin 100000 rand-0-10
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1.8M_mult2_I4.bin 1800000 mult2
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1.8M_mult3_I4.bin 1800000 mult3
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

./datagen/datagen_I4 data/1.8M_val_I4.bin 1800000 all10s
if [ $? != 0 ]; then echo "FAILURE on [$instr]"; exit 1; fi 

echo "TEST DATA SUCCESSFULLY GENERATED!!"
