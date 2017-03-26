OBJS   = inlining.o
CC     = gcc

.PHONY : clean

inlining : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

inlining.o : inlining.c inliningSep.c

clean:
	rm -f $(OBJS) inlining
