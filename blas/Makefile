CC=clang
GEM5=~/gem5/util/m5
CFLAGS=-g3 -pedantic -Wall -I ~/gem5/include/ -I $(GEM5)
LDLIBS=-lopenblas -lpthread -lm $(GEM5)/m5_mmap.o $(GEM5)/m5op_x86.o

.FORCE: ;

angle.in: .FORCE;
	./angle_gen.py > $@

%_s: %.c
	$(CC) $< $(CFLAGS) -DSIMULATED $(LDLIBS) -static -o $@

%.ll: %.c
	$(CC) $< $(CFLAGS) -DSIMULATED -emit-llvm -S -o $@

pythag_edit: pythag_edit_linked.ll
	clang $< -static $(LDLIBS) -o $@

pythag_edit_linked.ll: ~/sproj-git/mmiofu/fu_mmap.ll pythag_edit.ll
	llvm-link $^ -S -o $@

pythag_edit.ll: pythag.ll ~/sproj-git/passes/callrepl/libCallReplPass.so
	opt -load $(word 2, $^) -callrepl -S < $< > $@

