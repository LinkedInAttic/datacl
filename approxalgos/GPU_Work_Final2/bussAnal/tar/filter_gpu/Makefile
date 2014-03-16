NVCC 	= nvcc
CC 	= nvcc
LD	= nvcc

INC = -I/usr/local/cuda/samples/common/inc/
NVCCFLAGS 	= -arch=sm_20 -g -G
LDFLAGS	=
LIBS 	=

DIRS 	= $(subst /, ,$(CURDIR))
PROJ 	= $(word $(words $(DIRS)), $(DIRS))

TARGET    = $(PROJ) 
SRC_DIR   = src
OBJ_DIR   = obj
LIB_DIR   = lib

CPP_FILES = $(wildcard $(SRC_DIR)/*.cpp)
CU_FILES  = $(wildcard $(SRC_DIR)/*.cu)

H_FILES   = $(wildcard $(SRC_DIR)/*.h)
CUH_FILES = $(wildcard $(SRC_DIR)/*.cuh)

OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(notdir $(CPP_FILES:.cpp=.o)))
CUO_FILES = $(addprefix $(OBJ_DIR)/,$(notdir $(CU_FILES:.cu=.cu.o)))

OBJS =  $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(CPP_FILES)))
OBJS += $(patsubst %.cu,$(OBJ_DIR)/%.cu.o,$(notdir $(CU_FILES)))


$(TARGET) : $(OBJS)
	$(LD) $(LDFLAGS) $(LIB_CUDA) -o $@ $?

$(OBJ_DIR)/%.cu.o : $(SRC_DIR)/%.cu $(CUH_FILES)
	$(NVCC) $(NVCCFLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp $(H_FILES)
	$(NVCC) $(NVCCFLAGS) $(INC) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

