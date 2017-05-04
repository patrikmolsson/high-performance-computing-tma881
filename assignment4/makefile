SOURCE =  testcl.c
OUTPUT = testcl
CC = gcc
INCLOC = -I/usr/local/cuda-7.5/targets/x86_64-linux/include
LIBLOC = -L/usr/local/cuda-7.5/targets/x86_64-linux/lib
LIBS =  -lm -lOpenCL
OPTIM =
OTHERFLAGS =

main : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(INCLOC) $(LIBLOC) $(LIBS) $(OTHERFLAGS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(makefile|^$(SOURCE)$$)' | xargs rm -f
