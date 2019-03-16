CC=clang
CFLAGS=-g3 -pedantic -Wall
LDLIBS=-lopenblas -lpthread -lm

.FORCE: ;

angle.in: .FORCE;
	./angle_gen.py > $@

%_s: %.c
	$(CC) $< $(CFLAGS) $(LDLIBS) -static -o $@

%.ll: %.c
	$(CC) $< $(CFLAGS) -emit-llvm -S -o $@
