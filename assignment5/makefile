SOURCE = dijkstra.c
OUTPUT = dijkstra
CC = mpicc
LIBS =  -lm
OPTIM =
OTHERFLAGS =

.PHONY : all

all : dijkstra

dijkstra : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(LIBS) $(OTHERFLAGS)

$(OBJS) : $(SOURCE)

clean:
	@rm -f $(OUTPUT)
