OBJS   = main.o
SOURCE = main.c
CC     = gcc


.PHONY : clean

indirect_addressing : $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(OBJS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|$(SOURCE))' | xargs rm -f
