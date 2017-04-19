SOURCE = newton.c
OUTPUT = newton
CC = gcc
LIBS = -pthread -lm
OPTIM = -O3

.PHONY : all

all : main

main : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) -Wall

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|^$(SOURCE)$$)' | xargs rm -f
