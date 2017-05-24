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

test : mpi_test.c
	$(CC) -o mpi_test $(OPTIM) mpi_test.c $(LIBS) $(OTHERFLAGS)

$(OBJS) : $(SOURCE)

clean:
	#@ls | grep -v -E '(makefile|draw_graph.py|^$(SOURCE)|^$(SOURCE)$$)' | xargs rm -f
