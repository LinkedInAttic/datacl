CC=gcc -O4 -std=gnu99 
LINKOPTIONS = -lm -g

#CC = icc -O3 -std=c99 
#LINKOPTIONS = -ipp -DIPP -DCILK -lm -g

CFLAGS = -c -Wall

INCLUDES = -I ./includes/
TARGET_DIR = bin
SRC_DIR = src
TEST_DIR = test

SRCOBJS = bin/approx_unique.o\
	bin/assign_I4.o \
	bin/assign_I1.o \
	bin/mmap.o \
	bin/spooky_hash.o \
	bin/determine_rho_loc.o

all: testApproxUnique testSpookyHash

$(TARGET_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LINKOPTIONS) -o $@

$(TARGET_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) $< $(LINKOPTIONS) -o $@

testApproxUnique: $(SRCOBJS) bin/testApproxUnique.o
	$(CC) $(SRCOBJS) bin/testApproxUnique.o $(LINKOPTIONS) -o testApproxUnique.exe

testSpookyHash: $(SRCOBJS) bin/testSpookyHash.o
	$(CC) $(SRCOBJS) bin/testSpookyHash.o $(LINKOPTIONS) -o testSpookyHash.exe

clean:
	rm -f $(TARGET_DIR)/*.o *.exe 
