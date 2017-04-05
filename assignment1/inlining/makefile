SOURCE = main.c inliningSep.c
OBJS = main.o inliningSep.o
CC     = gcc

.PHONY : clean

main : $(SOURCE)
	$(CC) $(SOURCE) -o $@ $(CFLAGS)

clean:
	rm -f $(OBJS) main
