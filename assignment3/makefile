SOURCE =  cell_distances.c
OUTPUT = cell_distances
CC = gcc
LIBS =  -lm
OPTIM = -Ofast
OTHERFLAGS = -Wall -fopenmp
CFLAGSGP = -pg
CFLAGSGC = -fprofile-arcs -ftest-coverage

cell_distances : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) $(OTHERFLAGS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(report.markdown|makefile|^$(SOURCE)$$)' | xargs rm -rf


gprof : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGP) $(LIBS) && ./main_$@  && $@ main_$@ gmon.out

gcov : $(SOURCE)
	$(CC) $(SOURCE) -o main_$@ $(CFLAGS) $(CFLAGSGC) $(LIBS) && ./main_$@  && $@ $(SOURCE) && less $(SOURCE).$@
