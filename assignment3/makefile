SOURCE = cell_distances.c
OUTPUT = cell_distances
CC = gcc
LIBS =  -lm
OPTIM = -O0
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

.PHONY : all

all : main

cell_distances : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) -Wall

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|^$(SOURCE)$$)' | xargs rm -f


gprof : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) $(LIBS) && ./main_$@  && $@ main_$@ gmon.out

gcov : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) $(LIBS) && ./main_$@  && $@ $(SOURCE) && less $(SOURCE).$@
