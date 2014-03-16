CC=gcc -O4 -std=gnu99
LINKOPTIONS= -lm -g

#CC=icc -O3 -std=c99
#LINKOPTIONS=-ipp -DIPP -DCILK -lm -g

CFLAGS = -c -Wall

INCLUDES = -I ./includes/
TARGET_DIR = bin
SRC_DIR = src
TEST_DIR = test

SRCOBJS = bin/approx_quantile.o\
	bin/assign_I4.o \
	bin/assign_I1.o \
	bin/mmap.o \
	bin/determine_b_k.o \
	bin/qsort_asc_I4.o \
	bin/Collapse.o \
	bin/New.o \
	bin/Output.o \


all: testApproxQuantile

$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LINKOPTIONS) -o $@

$(TARGET_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC)  $(CFLAGS) $(INCLUDES) $< $(LINKOPTIONS) -o $@

testApproxQuantile: $(SRCOBJS) bin/testApproxQuantile.o
	$(CC)  $(SRCOBJS) bin/testApproxQuantile.o $(LINKOPTIONS) -o testApproxQuantile.exe

clean:
	rm -f $(TARGET_DIR)/*.o *.exe 
