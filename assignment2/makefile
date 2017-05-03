SOURCE = newton.c
OUTPUT = newton
CC = gcc
LIBS = -pthread -lm
OPTIM = -Ofast -march=native
CFLAGS = -Wall
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

newton : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) $(CFLAGS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(report.markdown|makefile|^$(SOURCE)$$)' | xargs rm -rf


gprof : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) $(LIBS) && ./main_$@  && $@ main_$@ gmon.out

gcov : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) $(LIBS) && ./main_$@  && $@ $(SOURCE) && less $(SOURCE).$@
