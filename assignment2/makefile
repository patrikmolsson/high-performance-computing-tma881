SOURCE = newton.c
OUTPUT = newton
CC = gcc
LIBS = -pthread -lm

.PHONY : all

all : main

main : $(SOURCE)
	$(CC) -o $(OUTPUT) $(SOURCE) $(LIBS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|$(SOURCE)|main.c|README)' | xargs rm -f
