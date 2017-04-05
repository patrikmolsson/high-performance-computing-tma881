SOURCE = main.c
OBJS = main.o
CC = gcc

.PHONY : all clean check

all : leak

leak : $(OBJS)
	$(CC) -o $@ $(OBJS)

$(OBJS) : $(SOURCE)

check : 
	valgrind ./leak  
clean :
	@ls | grep -v -E '(makefile|$(SOURCE))' | xargs rm -f

