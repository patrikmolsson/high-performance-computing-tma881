SOURCE = main.c
CC = gcc

.PHONY : locality clean full

locality : $(SOURCE)
	$(CC) -o $@ $(CFLAGS) $(SOURCE)

locality_full : $(SOURCE)
	$(CC) -o $@ -O3 $(SOURCE)

clean:
	rm -f locality locality_full
