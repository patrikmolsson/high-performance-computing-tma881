SOURCE = inlining.c inliningSep.c
OBJS   = inlining.o
CC     = gcc

.PHONY : clean

inlining : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS) : $(SOURCE)

clean:
	rm -f $(OBJS) inlining
