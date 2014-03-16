CC = nvcc
SRCSCPP = $(wildcard *.cpp)
SRCSCU = $(wildcard *.cu)
SRCS = $(SRCSCPP) $(SRCSCU)
HDRS = $(wildcard *.h)
OBJSCPP = $(SRCS:.cpp=.o)
OBJSCU = $(SRCS:.cu=.cu.o)
DIRS = $(subst /, ,$(CURDIR))
PROJ = $(word $(words $(DIRS)), $(DIRS))

APP = $(PROJ)
CFLAGS = -c -g -I/usr/local/cuda/samples/common/inc/ #-Wall
LDFLAGS =
LIBS =

all: $(APP)

$(APP): $(OBJSCU) $(OBJSCPP)
	echo $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJSCPP) $(OBJSCU) -o $(APP) $(LIBS)

%.cu.o: %.cu $(HDRS) $(MF)
	$(CC) $(CFLAGS) $< -o $@

%.o: %.cpp $(HDRS) $(MF)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(APP)
