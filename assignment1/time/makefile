OBJS = main.o
SOURCE = main.c
CC = gcc
OPTFLAGS = 0 1 2 3 s g
TIMES = 10

.PHONY : all clean time assembler

all : main

main : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

$(OBJS) : $(SOURCE)

time :
	@echo "Starting the timing. The program will be run $(TIMES) times for each optimisation flag in '$(OPTFLAGS)'."
	@$(foreach opt,$(OPTFLAGS), \
		$(CC) $(SOURCE) -O$(opt) -o main$(opt) ; \
		printf "\nTime for -O$(opt) flag:" ; \
		bash -c 'time for i in {1..$(TIMES)} ; do \
			./main$(opt) &> /dev/null ; \
		done'; \
	)

clean:
	@ls | grep -v -E '(makefile|$(SOURCE))' | xargs rm -f

assembler: $(SOURCE)
	@echo "Creating the assembly files for optimisation flags in '$(OPTFLAGS)'."
	@$(foreach opt, $(OPTFLAGS),\
		$(CC) $(SOURCE) -O$(opt) -o main$(opt).s -S; \
	)
