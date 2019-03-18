GEM5=$(HOME)/gem5

CC=clang
CFLAGS=-g3 -pedantic -Wall -I$(GEM5)/include
LDLIBS=-lopenblas -lpthread -lm $(GEM5)/util/m5/m5op_x86.o $(GEM5)/util/m5/m5_mmap.o

.FORCE: ;

angle.in: .FORCE;
	./angle_gen.py > $@

%_s: %.c
	$(CC) $< $(CFLAGS) $(LDLIBS) -static -o $@

%.ll: %.c
	$(CC) $< $(CFLAGS) -emit-llvm -S -o $@
