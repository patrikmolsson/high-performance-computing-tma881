SOURCE =  heat_diffusion.c
OUTPUT = heat_diffusion
CC = gcc
INCLOC = -I/usr/local/cuda-7.5/targets/x86_64-linux/include
LIBLOC = -L/usr/local/cuda-7.5/targets/x86_64-linux/lib
LIBS = -lOpenCL
OPTIM = -march=native -O3
.PHONY : all

all : heat_diffusion

heat_diffusion : $(SOURCE)
	$(CC) -o $(OUTPUT) $(OPTIM) $(SOURCE) $(INCLOC) $(LIBLOC) $(LIBS)

$(OBJS) : $(SOURCE)

clean:
	@ls | grep -v -E '(report.markdown|makefile|^$(SOURCE)|^$(SOURCE)$$)' | xargs rm -f
