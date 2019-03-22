CC=clang
CFLAGS=-g3 -pedantic -Wall -lm -fPIC

%: %.c
	$(CC) $< $(CFLAGS) -o $@

fu_mmap.ll: fu_mmap.c
	$(CC) $< -S -emit-llvm -o $@

fu_mmap.c: fu_mmap.h
# fu_mmap.h: fu_addr.h
# fu_addr.h: fu_addr.def
# 	python3 gen_fu_addr.py < $< > $@
