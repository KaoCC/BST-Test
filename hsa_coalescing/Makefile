ifndef HSA_RUNTIME_PATH
	HSA_RUNTIME_PATH=/opt/hsa
endif


CC=g++
TEST_NAME=bst
LFLAGS= -g  -Wall
INCS = -I $(HSA_RUNTIME_PATH)/include
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix obj/, $(notdir $(CPP_FILES:.cpp=.o)))

#COMMON_C_FILES := $(wildcard ../common/*.c)
#COMMON_OBJ_FILES := $(addprefix ../common/obj/, $(notdir $(COMMON_C_FILES:.c=.o)))

all: $(TEST_NAME)

$(TEST_NAME): $(OBJ_FILES) kernel.o
	$(CC) $(LFLAGS) $(OBJ_FILES) -O2  kernel.o -lelf -L$(HSA_RUNTIME_PATH)/lib -lhsa-runtime64 -o $(TEST_NAME)

kernel.o:
	cloc -c kernel.cl

obj/%.o: %.cpp
	$(CC) -c -O2 $(CFLAGS) $(INCS) -o $@ $< -std=c++11

clean:
	rm -rf obj/*o *.brig $(TEST_NAME) kernel.o


test:	
	echo "export LD_LIBRARY_PATH=$(HSA_RUNTIME_PATH)/lib" > test.sh
	echo "./$(TEST_NAME)" >> test.sh
	bash test.sh
	rm test.sh


