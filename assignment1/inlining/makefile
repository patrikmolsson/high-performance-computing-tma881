SOURCE = main.c inliningSep.c
OBJS   = main.o
CC     = gcc

.PHONY : clean

main : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS) : $(SOURCE)

clean:
	rm -f $(OBJS) inlining
