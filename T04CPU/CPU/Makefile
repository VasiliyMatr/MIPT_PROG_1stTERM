
INCLUDE_PATH_ = -Iheaders -I../DSL
SOURCE_PATH_  =  source/

GPP_FLAGS_    = -g -O0
OUT_NAME_     =  cpu

.PHONY: all build valg run cleanup

all: build

build: $(wildcard $(SOURCE_PATH_)*.cpp)
	g++ $(SOURCE_PATH_)cpu_main.cpp    $(SOURCE_PATH_)cpu.cpp \
		$(SOURCE_PATH_)local_utils.cpp $(SOURCE_PATH_)utils.cpp \
		$(GPP_FLAGS_) $(INCLUDE_PATH_) -o $(OUT_NAME_)

valg:
	valgrind --leak-check=full ./$(OUT_NAME_) data/asm.txt -o runnable

run:
	./$(OUT_NAME_) data/runnable