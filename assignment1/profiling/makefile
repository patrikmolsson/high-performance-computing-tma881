SFILE = main.c
DIR = ../locality/

SOURCE = $(DIR)$(SFILE)

CC = gcc
CFLAGS = -Wall
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

.PHONY : clean

gprof : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) && ./main_$@  && $@ main_$@ gmon.out

gcov : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) && ./main_$@  && $@ $(SFILE) && less $(SFILE).$@

clean:
	@ls | grep -v -E '(makefile)' | xargs rm -f
	@rm -f $(OBJS) $(DIR)main
