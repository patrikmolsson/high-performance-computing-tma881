OBJS = main.o
SOURCE = main.c
CC = gcc
OPTFLAGS = 0 1 2 3 s fast
TIMES = 1000

.PHONY : all clean time assembler

all : main

main : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

$(OBJS) : $(SOURCE)

time : $(OBJS)
	@echo "Starting the timing. The program will be run $(TIMES) times for each optimisation flag in '$(OPTFLAGS)'."
	@$(foreach opt,$(OPTFLAGS), \
		$(CC) $(OBJS) -O$(opt) -o main$(opt) ; \
		printf "\nTime for -O$(opt) flag:" ; \
		time for i in {0..$(TIMES)} ; do \
			./main$(opt) >/dev/null ; \
		done ; \
	)

clean:
	@ls | grep -v -E '(makefile|$(SOURCE))' | xargs rm -f

assembler: $(SOURCE)
	@echo "Creating the assembly files for optimisation flags in '$(OPTFLAGS)'."
	@$(foreach opt, $(OPTFLAGS),\
		$(CC) $(SOURCE) -O$(opt) -o main$(opt).s -S; \
	)
