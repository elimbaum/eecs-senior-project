CC=clang
CFLAGS=-g3 -pedantic -Wall
LDLIBS=-lopenblas -lpthread -lm

.FORCE: ;

angle.in: .FORCE;
	./angle_gen.py > $@

angle_s: angle.c
	$(CC) $< $(CFLAGS) $(LDLIBS) -static -o $@

angle.ll: angle.c
	$(CC) $< $(CFLAGS) -emit-llvm -S -o $@
