NVCC 	= nvcc
#using gcc for the c files since nvcc does not accept some arguments used in CFLAGS below
CC 	= gcc
CPP     = g++
LD	= nvcc

# INC = -I./rs_src/
#cuda include dir
INC += -I/usr/local/cuda-5.5/include/

NVCCFLAGS = -arch=sm_20 -g -G
CFLAGS = -Wall -std=gnu99 -g
CPPFLAGS = -Wall -std=c++0x -g

NVCCFLAGS = -arch=sm_20 -O3
CFLAGS = -Wall -O3 -std=gnu99
CPPFLAGS = -Wall -O3 -std=c++0x

LDFLAGS	= -lcurl -lm -lboost_filesystem-mt
LIBS 	=

DIRS 	= $(subst /, ,$(CURDIR))
PROJ 	= $(word $(words $(DIRS)), $(DIRS))

TARGET     = $(PROJ) 
SRC_DIR    = src
OBJ_DIR    = obj
LIB_DIR    = lib

CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
C_FILES = $(wildcard $(SRC_DIR)/*.c)
CU_FILES  = $(wildcard $(SRC_DIR)/*.cu)

HPP_FILES   = $(wildcard $(SRC_DIR)/*.hpp)
H_FILES   = $(wildcard $(SRC_DIR)/*.h)
CUH_FILES = $(wildcard $(SRC_DIR)/*.cuh)

OBJS =  $(patsubst %.cpp,$(OBJ_DIR)/%.cpp.o,$(notdir $(CPP_FILES)))
OBJS +=  $(patsubst %.c,$(OBJ_DIR)/%.c.o,$(notdir $(C_FILES)))
OBJS += $(patsubst %.cu,$(OBJ_DIR)/%.cu.o,$(notdir $(CU_FILES)))

.PHONY : all
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(LD) $(LDFLAGS) $(LIB_CUDA) -o $@ $^

$(OBJ_DIR)/%.cu.o : $(SRC_DIR)/%.cu $(CUH_FILES) $(HPP_FILES) $(H_FILES)
	$(NVCC) $(NVCCFLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR)/%.cpp.o : $(SRC_DIR)/%.cpp $(HPP_FILES) $(H_FILES)
	$(CPP) $(CPPFLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR)/%.c.o : $(SRC_DIR)/%.c $(H_FILES)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

.PHONY : clean
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

