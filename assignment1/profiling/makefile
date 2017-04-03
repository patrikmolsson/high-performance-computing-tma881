SOURCE = main.c
OBJS = main.o
CC = gcc
CFLAGS = -Wall
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

.PHONY : all clean

all : main

main : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS) : $(SOURCE)

gprof : $(OBJS)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) && ./main_$@ && $@ main_$@ gmon.out

gcov : $(OBJS)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) && ./main_$@ && gcov $(SOURCE) && less $(SOURCE).gcov

clean:
	@ls | grep -v -E '(makefile|$(SOURCE)|README)' | xargs rm -f && rm -f *.gcov
