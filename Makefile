CC=gcc
CFLAGS=-m32 -g
CLIB=-I./include -L./lib -lhardware
EXEC=mi_kernel


all: $(EXEC)

mi_kernel: mi_kernel.o
	$(CC) -o $@ $^ $(CFLAGS) $(CLIB)

.PHONY: clean realclean test

clean:
	rm -rf *~ *.o

realclean:
	make clean
	rm -rf $(EXEC) vdisk*.bin
