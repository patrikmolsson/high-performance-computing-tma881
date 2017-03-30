SOURCE = main.c
OBJS = main.o
CC = gcc

.PHONY : all clean

main : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS) : $(SOURCE)

clean:
	rm -f $(OBJS) main
