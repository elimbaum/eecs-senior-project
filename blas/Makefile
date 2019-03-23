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

pythag_edit: pythag_edit_linked.ll
	clang $< -lm -o $@

pythag_edit_linked.ll: ~/sproj-git/mmiofu/fu_mmap.ll pythag_edit.ll
	llvm-link $^ -S > $@

pythag_edit.ll: pythag.ll ~/sproj-git/passes/callrepl/libCallReplPass.so
	opt -load $(word 2, $^) -callrepl -S < $< > $@

