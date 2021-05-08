
INCLUDE_PATH_ = -Iheaders -Idsl
SOURCE_PATH_  = source/
DATA_PATH_    = data/

GPP_FLAGS_    = -g -O0

PROG_NAME_    = cmpl-debug
IN_NAME_      = code.txt
OUT_NAME_     = ../../T04CPU/ASM/data/asm.txt

.PHONY: all build valg run cleanup

all: build

build: $(wildcard $(SOURCE_PATH_)*.cpp)
	g++ $(SOURCE_PATH_)*.cpp $(GPP_FLAGS_) $(INCLUDE_PATH_) -o $(PROG_NAME_)

valg:
	valgrind --leak-check=full ./$(PROG_NAME_) $(DATA_PATH_)$(IN_NAME_) -T -o $(OUT_NAME_)

run:
	./$(PROG_NAME_) $(DATA_PATH_)$(IN_NAME_) -T -o $(OUT_NAME_)

cleanup:
	rm *.dot *.png *.txt