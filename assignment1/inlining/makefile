SOURCE = main.c inliningSep.c
OBJS = main.o inliningSep.o
CC     = gcc
OPTIMFLAG = -O3

.PHONY : clean

inlining : $(SOURCE)
	$(CC) $(SOURCE) -o $@ $(CFLAGS) $(OPTIMFLAG)

clean:
	rm -f $(OBJS) inlining
