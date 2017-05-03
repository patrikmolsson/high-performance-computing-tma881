SOURCE =  cell_distances.c sse_mathfun.h
OUTPUT = cell_distances
CC = gcc
LIBS =  -lm
OPTIM = -Ofast -msse4.2
OTHERFLAGS = -Wall -fopenmp
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

.PHONY : all

all : main

main : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) $(OTHERFLAGS) -I.

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|^$(SOURCE)$$)' | xargs rm -f


gprof : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) $(LIBS) && ./main_$@  && $@ main_$@ gmon.out

gcov : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) $(LIBS) && ./main_$@  && $@ $(SOURCE) && less $(SOURCE).$@
