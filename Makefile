TARGET_EXEC = dcc
CC = clang++

SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard src/**/**/*.cpp) $(wildcard src/**/**/**/*.cpp)
TEST = $(wildcard test/tests/*.c)
TESTOBJ = $(TEST:.c=.S)
OBJ = $(SRC:.cpp=.o)
ASM = $(SRC:.cpp=.S)
BIN = bin
TESTDIR = test

INC_DIR_SRC = -Isrc
INC_DIR_LIB =

DEBUGFLAGS = $(INC_DIR_SRC) $(INC_DIR_LIB) -Wall -g
RELEASEFLAGS = $(INC_DIR_SRC) $(INC_DIR_LIB) -O2
ASMFLAGS = $(INC_DIR_SRC) $(INC_DIR_LIBS) -Wall
LDFLAGS = $(LIBS) -lm -fuse-ld=mold

.PHONY: all libs clean test

all: 
	$(MAKE) -j8 bld
	$(MAKE) link

dirs:
	mkdir -p ./$(BIN)

link: $(OBJ)
	$(CC) -o llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native $(BIN)/$(TARGET_EXEC) $^ $(LDFLAGS)

bld: 
	$(MAKE) clean
	$(MAKE) dirs
	$(MAKE) obj

obj: $(OBJ)

asm: cleanassembly $(ASM)

%.o: %.cpp
	$(CC) -std=c++20 -o $@ -c $< $(DEBUGFLAGS)

%.S: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	@echo 'Building ASM'
	$(CC) -std=c++20 -S -O -o $@ -c $< $(ASMFLAGS)
	@echo 'Finished building: $<'
	@echo ' '

%.S: %.c 
	./$(BIN)/$(TARGET_EXEC) $< $@

build: dirs link

run:
	./$(BIN)/$(TARGET_EXEC) test.c test.S

dltest:
	rm -rf $(TESTOBJ)

test: all
	$(CC) -std=c++2a -o $(TESTDIR)/testbuild $(TESTDIR)/tmain.cpp
	./$(TESTDIR)/testbuild

test1: run 	

testasm:
	$(CC) -o $(TESTDIR)/test.o -c test.S 
	clang -o $(TESTDIR)/main.o -c $(TESTDIR)/main.c 
	$(CC) -o $(TESTDIR)/test $(TESTDIR)/main.o $(TESTDIR)/test.o 
	./$(TESTDIR)/test

clean:
	clear
	rm -rf $(BIN) $(OBJ)

cleanassembly:
	rm -rf $(ASM)
