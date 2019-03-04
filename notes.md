# Senior Project Notes

 Eli Baum '19

Advisor: Rajit Manohar

## General TODO

- [ ] read FPGA paper?
- [x] read ADRES & other CGRA paper (dynamic translation!)
  - [ ] Look at ADRES sources
- [x] ...including DRSEC
- [ ] look into how shared objects work, esp wrt BLAS
  - [x] might not be necessary with JIT – keep it simple – lookup by name.
- [ ] profiling
- [ ] read about LLVM backends/assembly generation
- [ ] LLVM writeup **don’t forget about this!**
- [ ] **look at Wiki Dynamic Translation bib!**
  - [ ] Dynamo
  - [ ] CGRA dynamic paper bib
- [ ] MorphoSys/TinyRISC: http://gram.eng.uci.edu/morphosys/

## Project TODO

- [ ] LLVM write up
- [ ] ARM in gem5
- [ ] LLVM cross compile to ARM
- [ ] JIT
- [ ] BLAS
- [ ] add hardware feature in gem5


## Important Dates

12 Sep: Prospectus Due

Reading period: presentation

## Project Notes

Amdahl's Law may limit the effectiveness of this project.

$S = \frac{1}{(1-p) + \frac{p}{s}}$

where $S$ is overall speedup, $p$ is proportion of time in critical section, and $s​$ is specific speedup.

So if some component is run 1/2 time, and I speed it up 2x, that’s a 33% speedup. Not horrible.

Why is this different from ADRES? I should do some more research into this, but ADRES fundamentally is pre-compilation and thus limited by the same kind of issues that any JIT solves. I'm kind of moving ADRES into a JIT.

JIT will take LLVM IR (/bitcode, really) and run it. JIT config will tell about current hardware capabilities available. This instruction should actually compile to this object code; these arguments correspond to these registers, return value. Only "software" that is not platform-specific is the configuration file. Not only does this not require rewriting compilers, but it doesn't even require recompiling code.

I think, rather than creating a new JIT layer (though I could), I may just want to use the existing IRTransform layer. custom function: supplies module as argument, returns module.

Precedence: if function is defined in current module, do not replace it. If external symbol, call it. Maybe could even add some kind of token to mark functions to never be replaced.

I have to look into the code generation part, not the optimization part. In the example given, they show how the PowerPC backend knows to map LLVM `fadd` to PPC `FADDS` and LLVM `fadd fmul` to `FMADSS` (mul & add).

Memory mapped IO should work.

Rajit mentioned that we should build up: start with simpler functions, etc. But, at least looking at BLAS source, it doesn't seem like there's necessarily a lot of hierarchy. Sure, function calls are expensive, so it may not be that obvious. Perhaps vector addition & dot product engines would be good.

For example, SAXPY is constant * vector + vector. SDOT is vector • vector.

(also, I'm only going to be doing real integers)

ok, now that i’ve learned about gem5, it looks like:

- implement JIT that can recognize certain function calls (either literally or semantically)
- for those instructions, JIT emits memory mapped IO (write/interrupt/read?)
- modify gem5 CPU to include FU at that memory mapped IO location, possible interrupt

Seems like the CPU doesn’t actually to be modified much. Really what I need to do is hookup a peripheral… found someone’s thesis from 2012. will read. not sure how updated. (vs. current state of gem5)

The memory-mapped IO is something in the full-system config file. So it will be something like the previous port connections, need to tell the memory to forward certain ranges to the FU. Rui has figured out the MMIO, at least on the O3 cpu. does it matter? of course O3 will be more efficient, but is that going to have a bearing on my results?

gem5 has limited RISC-V support (no full system)

hey! scons is silly. not actual folder structure. `scons build/RISCV/gem5.opt` compilation issue with template function includes? Trying to install with a fresh pull from git.

systemC _is_ required, have to load GCC 7.3.0. ohhh in meeting we said RISC might be tricky. my bad. so maybe i’ll try it all again with ARM.

It seems like everything is loaded correctly. I don’t know why it’s not working. Trying in local lubuntu VM. Looks like zoo works, however. Local lubuntu also works. **Moving to zoo**

Guess that means I need to reinstall LLVM locally on zoo. lol. git & configs moved over.

### Installing LLVM on Zoo

- Install LLVM on zoo
- Set up LLVM cross-compilation
- Cross compile test program
- Run in gem5

`cmake -G "Ninja" -DLLVM_ENABLE_PROJECTS=clang -DCMAKE_INSTALL_PREFIX=~/sproj/llvm ../llvm`

`cmake --build .`  (automatically calls Ninja for parallel build) linking is still slow.

`cmake --build . --target install`  to move everything into place

shoot, gotta install clang as well… ENABLE PROJECTS.

it works! kaleidoscope, etc.

### Cross Compiling

Using the `arm-linux-gnueabihf` tools, I can cross compile (just have to remember to `-static` because simulated system doesn’t have the libraries included). However, can’t figure out how to do that with LLVM. Rui will send me config tmrw.

## FU Notes

Creating the FU. How many functions to do? Look at a couple:

* SSCAL: x=a\*x. scalar times vector.
* SNRM2: euclidean norm. sqrt dot product. (vector times vector)
* SDOT: dot product. vector times vector.
* SAXPY: y=a\*x+y. scalar times vector plus vector.

so the operations needed are:

- scalar times vector: parallel
- dot product: partially parallel
- vector addition: parallel

vector size: make these support X bytes, also send length? probably easier than special end-of-vector value, especially if i’m doing integers. anyway, this is how BLAS does it. might make interface easier. direct-copy memory.

lots of this can be parallelized, but depends how many multipliers & adders i want. could have single full adder/multiplier with multiplexer. even having two of each could be cool though. then even dot product can take advantage of it for the summation part, at least partially.

With ACT/CHP templates, can easily do N-bit M-wide vectors, so don’t have to worry about bitwidths too much (as far as design goes), only matters when physically sizing out the chip.

I can pull these from other sources. get a reasonably efficient multiplier/adder. Can also try parallel/nonparallel compare result.

From talking to zeb, doing with a single ALU will get tricky. his compiler just puts in a new adder for each expression. no CSE, not even any reuse. that’s tricky. however, in a loop, might not be terrible. We didn’t have arrays, though… or maybe we could.

templated adder

but can’t do templated adder - adder.

basic sketch:

```
// dot product ACT
// this will construct N multipliers and N-1 adders, all of bitwidth B

template<pint B, pint N>
defproc sdot(aN1of2<B> x[N]; aN1of2<B> y[N]; aN1of2 out[B])
{
	// TODO this was originally N > 3. Can it actually handle 2 & 3?
	[ N > 1 ->
		aN1of2 outL[B], outR[B];
		sdot<B, N/2>   L(x[0..N/2-1], y[0..N/2-1], outL);
		sdot<B, N-N/2> R(x[N/2..N-1], y[N/2..N-1], outR);
		int_add<B>(outL, outR, out);
	[] else ->
		// N = 1. just component-wise product, to be added
		int_mul<B>(x[0], y[0], out);
	]
}
```

What about the others? Scalar-vector mult and vector add. I think more straightforward:

```
template<pint B, pint N>
defproc sscal(aN1of2<B> a; aN1of2<B> x[N]; aN1of2<B> out[N])
{
	(i:N: int_mul<B>(a, x[i], out[i]); )
}

template<pint B, pint N>
defproc vector_add(aN1of2<B> x[N]; aN1of2<B> y[N]; aN1of2<B> out[N])
{
	(i:N: int_add<B>(x[i], y[i], out[i]); )
}

template<pint B, pint N>
defproc snrm2(aN1of2<B> x[N]; aN1of2 out[B])
{
	dualrail outSq[B];
	sdot<B, N>(x, x, outSq);
	sqrt<B>(outSq, out); // TODO
}
```

Much more straightforward. So then saxpy is just sscal and vector add:

```
template<pint B, pint N>
defproc saxpy(aN1of2<B> a; aN1of2<B> x[N]; aN1of2<B> y[N];
			  aN1of2<B> out[N])
{
	aN1of2<B> ax[N];
	sscal<B, N>(a, x, ax);
	vector_add<B, N>(ax, y, out);
}
```

neat.

(now just have to figure out the B-width operators themselves…) and also sqrt.

## Meeting Notes

### 30 Aug

Design abstraction: how to specify?

Memory: latency v throughput. Superscalar: out of order processors

Huffman & JIT: new instruction encoding, new functional units (in hardware)

### 10 Sep

new ideas:

- compressed memory system in general, not just instructions
- compressing entire functional blocks: DAISY + huffman
- changing huffman coding during execution? optimizing code during execution. two level optimization.
- open source compiler with JIT: LLVM (LLI).
  - *Read up more on JIT.*
- Huffman coding the bus
- switches on the function units? At runtime, configure the chip.

  - data bus is a one level tree. but it can also be multi-level. section functional units off into
  - balanced binary tree by default, but move the nodes around. programmable logic.
- Can my firmware figure out what I'm doing? Each functional block swapped in

  - Matrix multiply, linear algebra, FFT
  - BLAS library for linear algebra.
  - shared object library
- "Operand-Based Circuit Specialization"

  - cross between a CPU and an FPGA, but the FPGA programming is done at run time not compile
- berkeley numerical algebra matrix matrix multiply for different machines
- simulate new CPU.
- LLVM

  - do it on grace
  - build own version of LLVM.
- reread DAISY.
- email to get wiki account

### 20 Oct

OS can do linking

LLVM: use LLVM IR on chip, not code generation. JIT (LLI) generates the actual code. LLVM IR is architecture independent. IR would still have labels. easier to recognize patterns. even so, without this, dynamic linker would see labels.

Do this, and loader doesn't depend on hardware. Machine independent binary. New "firmware" but software is the same.

Work on software/firmware first.

Functional blocks might be broken down – multiplier and adder, know topologies for each operation.

LLI + IR benchmark. Rui Li has been working on this, talk to him.

storage@yale.

LLVM documentation to get started. they now have a RISC backend.

reference implementation for RISC, this is for later.

benchmarks: [linpack](http://www.netlib.org/benchmark/hpl/). (or LAPACK) later, would be interesting to do mixed code, linear algebra and not. look for uniprocessor implementation. need to get BLAS/base package, as well as benchmarks.

basic: count instructions, figure out how many could be saved by doing hardware optimization.

### 17 Dec

USE MORE CORES!

**Spend a week writing up how LLVM works. Just to think about it.**

**Do the ACT file for a block.**

Hardware unit tells JIT what units I have. JIT doesn't care.

Easy solution: strict match to instruction. Harder: recognize more complex structures.

emulate circuit on RISC. 

### 28 Jan

How to create CPU/FU interface? Probably can't create a custom instruction. Put it on a bus? How do peripheral busses work? Shared memory, with some kind of control signal? I'm only operating on the IR/ASM interface, so the output of my JIT needs to be standard assembly.

out of order RISCV implementation: BOOM.

Rui is working on RISC JIT: codegen for FPGA. backend works on LLVM, but JIT not implemented on RISCV. something with runtime linking.

I could use ARM, RISC might be tricky.

simulation, or FPGA (but FPGA requires RISC). so probably simulation, for now, unless someone fixes. **Gem 5** simulator, ARM. can't do async, so might have to fudge async. actually simulating async might be a big endeavor.

trampoline code: jump to jump

Step 1: get GEM5 running LLVM code. **cross-compilation**. (slow! checkpoint.)

Step 2: running my JIT on GEM5.

Step 3: add hardware feature (how to modify Gem?)

memory mapped IO. coordinate with Rui on what memory system should look like.

write in CHP, data flow simulator, add to simulator.

implement integer operation. start at the bottom (simple stuff, vectors).

### 28 Feb

**Questions**

- Is my sketch an appropriate use of templates?
  - Single ALU/operator blocks for N-wide vectors, or multiple? Much more complicated storage scheme required
  - make operators (sort/+/*) templated bundled data?
- Why RISCV over [Alpha or ARM](http://gem5.org/Supported_Architectures)?
  - No, I’m doing ARM silly
- Check TODO

ask Rui about ARM. also has MMIO working.

Dot products COULD be bigger than the circuit.

ACT is describing FIXED hardware. JIT knows about new functionality, and what it is being asked of it. could have different sized CHIPS, but not software-dependent.

MAC unit! (multiply accumulate)

hardware sqrt is cheap (in float). don’t worry about arithmetic @ moment.

memory bandwidth limits that! probably should MMIO pass pointer (shared memory). however, this CAN be faster, because load/store is operating on single words. But we could operate on whole cache line. MMIO can also send commands – which operation, pointers to arguments, and length.

LINALU should have its own cache.

2-way set associative, for each arguments. also: hardware stream prefetcher. looks at cache misses, and if it realizes a sequence, prefetch rest of the array (maybe).

a*x + y

don’t worry about ACT. do it all in GEM5.

Cache usually gives you a single word – look into giving up a whole line. this is technologically feasible but not the usual method.



## Reading Notes

### Benes Nowick Wolfe: Huffman

huffman-compress instructions for storage. decompressed when brought into cache. This is good for async because we already are trying to optimize for most common cases, not worst case scenario.

Decoder determines length of current symbol, and shifts down compressed cache line to next symbol. Shift Unit shifts 0-2 bytes, Alignment Network shifts 0-7 *bits*. Adder adds lengths of each code word, to know how much to offset for next. Furthermore, compute carry bit FIRST, so that we can shift, don't worry about sum except for bit-level offsets.

New design: self-timed ring + zero-overhead design to allow very small latency. Run match class, specific instruction, and decode in parallel! Instead of completion detector, use worst-case delay through each block.

*We're using dual rail-logic here.* Two wires per signal. Carry lookahead adder.

PKG: propagate, kill, generate.

This is really cool!

**Questions**:

- How do we set the huffman coding? Based on past examples of code?
- Clarification on self-timed ring?
- (Review Huffman codes)
- Why do we use matched delay? "Worst case path." Is it that its only for *that* functional block?
  - yes, and generally only for infrequent operations
- Why feed in_clk and !in_clk into SR latch?
  - SR latch prevents both lines being high (both low is ok)

### Ebcioglu Altman: DAISY

software conversion of old instruction sets. trying to maintain parallelism and keep overhead minimal. memory mapping (regular instructions => VLIW)

the *issue* of VLIW is that it is not compatible with existing software. We want new system to handle translation without recompilation.

running a firmware VM, essentially. Will cache previous translations.

### ADRES

Coarse-grained reconfigurable architecture (CGRA). Instead of configurable bit-level function units (FPGA), word or subword units

90/10 rule: 90% of time spent executing 10% of code... remaining 90% of the code can run of the regular processor just fine

ADRES is effectively a coprocessor. transparent usage; many shared resources. Important that *memory is shared here*... simplifies context switch.

This requires a new compiler -- doesn't dynamically translate. There are many of these but this is not want I actually want to do. But paper emphasizes that having a good software toolchain is very important for architecture adoption. If no additional software toolchain is required, this is a boon for a new system!

**DRSEC**: the compiler for ADRES. (separate paper)

"we believe a major strength of coarse-grained reconfigurable architectures is in loop-level parallelism." or some other kind of parallelism. I will need to look into parallelism on my project.

Added a special PHI instruction to the instruction set – which branch did program flow come from.

### Dynamic CGRA

"Despite their potential benefit, CGRAs face a major adoption challenge as they do not execute a standard instruction stream." this is what I would like to fix!

DORA: Dynamic Optimization for Reconfigurable Architectures

this is good, I think this is just about what I'm trying to do.

Look at the existing dynamic binary translation efforts cited in this paper -- they may not have been particularly promising, but they may have interesting insights/guidance.

next-executing tail (NET) algorithm: find optimization candidates such as tight loops. NET works by identifying BACK branches, which are usually loops (cool!). If that code can be optimized (?) store PC in cache and substitute in optimized hardware.

*There is an optimization to NET, and more discussion of it w.r.t. Dynamo, on the AVLSI wiki bib.*

### NET

watch for back branches to find loops/recursion => hot paths. different storage possibilities; either static code analysis & spanning tree or branch encoding. this is similar to coverage metic. take missed opportunity cost into account: sunk cost, kinda? if we've spent a long time trying to optimize this, ignore

this only works on programs that do have a lot of hot paths and reuse. that's what i should be testing on.

### AVLSI Wiki

C to FPGA might be useful for figuring how to read code. Though perhaps they are only using C programs, not compiled C programs.

I think I *should* look at CGRA.

### FPGAs

Reading from [this chapter](https://www.springer.com/cda/content/document/cda_downloaddocument/9781461435938-c2.pdf?SGWID=0-0-45-1333135-p174308376) and [this paper](https://www.hindawi.com/journals/ijrc/2009/259837/).

FPGAs use LUTs, which are really just RAM blocks indexing a certain logical function through multiplexers. Not sure that this is what I want – maybe just pass transistors.

Island style FPGA (aka mesh). logic block "islands" among routing connections. A large majority of the area is routing, so it may make more sense to optimize that!

Hierarchical FPGA: blocks grouped into clusters.

HSRA? Reconfigurable

### Hardware Routing

Dehon, Huang, Wawrzynek

software optimizations to routing is not great. hardware can assist. authors do NOT exploit task-specific routing.

HSRA: number of switches linear in number of endpoints. unique switch box set for every connection

HSRA switches: T-switch (3), π switch (4). For this routing, add circuitry to the T-switch to back-propagate source/sink channels. NEAT! If switch has NOT been configured yet, it passes logical OR of its children to the parent. Use (historical) congestion to decide which path to choose. Or random.

I think this optimization strategy is not super related to my project, but good to see some work on routing nonetheless. Victimization might be something I have to look at; my routing will probably be entirely in hardware unless I write a ROM program

### Profiling

How to figure out what is going on? LZW strikes me as a very simple idea (and count usage of each; throw away unused).

Machine learning? I hate to say it, but this is the kind of thing it would be good for. What I'm really trying to do here is topical decompilation – what general operations are being performed. More specifically, looking at BLAS, we're trying to identify certain function signatures. But this needs to be ON-CHIP! it also needs to be very good. can't risk accidentally using the wrong functional unit. Maybe this is not the best idea.

### LLVM Lang Ref

From http://llvm.org/docs/LangRef.htm

LLVM IR can be used as a bitcode, for use with a JIT. perfect. (maybe. how do we do subroutines? tbd)

`@` is global, `%` is local.

This is a bit dry. Might be best to work with examples.

### [Kaleidoscope: LLVM Tutorial](https://llvm.org/docs/tutorial/LangImpl01.html)

Creating a language.

Can't compile?

```
/tmp/kld-271d78.o:(.data+0x0): undefined reference to `llvm::EnableABIBreakingChecks'
```

lib/Support/Error.cpp isn't being loaded. abi-breaking.h is, but the extern is in there.

Trying to recompile. May have messed something up?

`-DLLVM_DISABLE
_ABI_BREAKING_CHECKS_ENFORCING=1` on compile fixes it. May need future fix if I actually need to include these libraries (this flag allows the headers to be included, without linking the actual library itself.) Chapter 3 compilation actually fixes this. `llvm-config` can include all the right libraries.

Fixed the prompt-printing bug. need to keep track of when we have completed an expression or not.

Now I can't link chapter 4. Things being included, but not found by the linker? looks like it's a linking issue. linking all libraries and it seems to work. But it's really slow. More cores?

Function redefinition issue: I don't think i'm storing both. Prototype is being updated (# args), but not body. so new function is not being stored. or is not being found correctly? the storage process involves a lookup, i think. not sure. i should just continue. might fix itself later on in the tutorial.

Build instructions in the tutorial are incomplete. Referring to cmakelists might work.

Extern issue might be caused by master. Try reinstalling 7.0.1? Works on 6.0.1...

Ok! Done with the Kaleidoscope tutorial. Now moving on to JIT.

Should look at: https://releases.llvm.org/6.0.1/docs/WritingAnLLVMPass.html to make a new pass!

### LLVM JIT Tutorial

https://releases.llvm.org/6.0.1/docs/tutorial/BuildingAJIT1.html (need to use the appropriate version)

Note about symbol resolvers:

> It may seem as though modules added to the JIT should know about one another by default, but since we would still have to supply a symbol resolver for references to code outside the JIT it turns out to be easier to re-use this one mechanism for all symbol resolution.

So even though it is not external, we treat it as such. Internal means *within the current module*, only.

Adding optimization. Current implementation is eager-compiler. Lazy/eager tradeoff.

Ok, so making a JIT is really straightforward. With not very much code at all I have a fully functional JIT.

What I need to do now, probably, is to create a new ORC JIT layer. This layer will read the configuration data, and decide if a given function should be *looked up in the JIT, or called out.*

### Writing an LLVM Pass

https://releases.llvm.org/6.0.1/docs/WritingAnLLVMPass.html

This is what I need to do! Ideally, I want a pass that, for certain library functions, emits code for the internal FU, instead of actually calling that function.

Need to make a shared object...

.ll is LLVM IR, .bc is LLVM bitcode. only generation difference is if `-S` or `-c` is used when compiling.

I probably want a `FunctionPass`. Not sure how that will work with the call... is it called before/during/after? Doesn't matter either way – runs on all functions. or `BasicBlockPass`. This just depends on where exactly the pass is called. I don't think I want `MachineFunctionPass` because by that point it may be too late. Side note: maybe I don't want to be looking at optimization passes, but at code generation... https://www.llvm.org/docs/CodeGenerator.html:

![image-20190127231319973](/Users/elibaum/Documents/senior project/etc/codegen-screenshot.png)

This is exactly what I want!

### LLVM Codegen

https://www.llvm.org/docs/CodeGenerator.html or https://releases.llvm.org/6.0.1/docs/CodeGenerator.html

Ok, I think I've finally found it. What I really want is just a complicated version of the pattern above, along with some complex code generation to allow arbitrary interface... memory mapped IO?

`MachineBuilderInstr`: create a custom instruction. can have arbitrary pattern matching – pass to function, rather than specifying simple pattern rules. or... `TargetInstructionInfo`. That's where all the instructions are actually picked up.

Finally, I would need to get in the way of the function call. So maybe actually it would be a combination of Codegen and optimization pass? Next step, I think is actually to figure out *where the linking of external objects happens*, and intercept that.

How to do with mutual inclusion? If optimized library function is also called within the library, JIT needs to somehow handle that library... problem as mentioned above. Would be good to intercept these calls as well.

If necessary, could allow override of matching functions – with a keyword or something.

### LAPACK User Guide

http://www.netlib.org/lapack/lug

LAPACK is largely a wrapper to the lower-level BLAS. LAPACK includes a reference implementation of BLAS, which is non-optimized, but that's ok for our purposes.

http://www.netlib.org/lapack/lawnspdf/lawn81.pdf quick install guide. Installed and tested! Mostly working, only one function failed to pass threshold.

### Grace Setup

SSH is disconnecting. Is this an interactive limitation? switch to pi_manohar

get git set up; a repo in ~/projects/

to install LLVM: had to `module load ...` to get Cmake and Ninja. (this is now in my bash profile) Then just following instructions here http://releases.llvm.org/7.0.0/docs/GettingStarted.html

look at Rajit's VNC thing? **tmux**/screen?

Using Tmux. I should change the prefix to something easier to type – maybe ^A.

`cmake -G Ninja -DCMAKE_INSTALL_PREFIX=~/project/llvm ~/project/llvm-src/` to generate build file

`ninja` to build (in parallel, by default)

starting over, got some weird error about aarch64_neon_fmlal2 intrinsics. Trying clang instructions http://clang.llvm.org/get_started.html instead. Just LLVM and clang. Huh, don't know why it didn't work before. success!

Reading: http://llvm.org/docs/LangRef.html LLVM language ref

Now looking to install BLAS.

Updated `interactive` to request 7-day idle timeout. LLVM is now using a TON of memory to build??

I don't really want to do VNC.

Trouble launching interactive sessions: because maintenance falls within my default 7-day allocation.

### gem5

I will use gem5 to simulate an ARM CPU. (wait, ARM? - 27 Feb)

http://learning.gem5.org/book/intro.html

Need to recompile for different architectures! Going to follow the tutorial, which does x86. But later will need to do RISC.

Weird build fail. Probably ran out of memory. Trying again not with full parallelization... (4 cores vs 14) Slow to install, but I have time. build success! took 30 minutes or so. maybe longer, wasn't keeping track. in the future, i should slurm building.

full system vs sys call: emulate the entire hardware system, or just a quasi-VM. SE much faster, does not require all hardware devices to be created. not sure which i'll need.

It works!

Adding a new object. Needs to be included in the actual source tree so it can be recompiled. Existing tutorial example removed by renaming `SConscript`.

call modes: atomic (fast forward?), functional (loading memory contents), timing (normal simulation)

writing cache. cache works! faster (small cache, 57M ticks; big cache, 33M ticks) and better hit ratio (91% vs 96%).

trying full system emulation. and we’re booting! not too tricky. but binaries and disks have to be in the gem5 root; all other files can be in configs/…

That part of `x86.py` that looks like a typo isn’t, it is supposed to be a local function.

Everything works! wow. & done with the Gem5 tutorial.

Next I should figure out how gem5 is executing instructions. looks like for the AtomicSimpleCPU, that code is in src/cpu/simple/atomic.cc and base.cc

There’s a preExecute() and postExecute(), and then StaticInstPtr::execute. can’t find where that’s defined. minor/execute.cc? That looks promising. execute part of the fetch/decode/execute cycle

Part 4 PPT has some good information.

Timing CPU much slower than atomic (actually simulates memory/CPU timings)

### RISC - gem5 thesis

https://cfaed.tu-dresden.de/files/Images/people/chair-cc/theses/1808_Scheffel.pdf

WRONG ARCH lol

### arm gem5 starter kit

it’s working! SE + FS. For FS, have to specify kernel NAME. It knows where to look, I guess based on M5_PATH variable.

minor CPU model not working. simulation just ends during boot. and with multiple cores, hands. AtomicSimple might? looking good… ~10 min boot. yay!

atomic is the fastest! instantaneous memory access. good for fast-forwarding. **NOTE:** different CPU models can be used for different parts of a simulation thanks to checkpoint. So can boot atomic, then run programs in minor. MinorCPU is a much more realistic simulation, so it’s incredibly slow. Pipelined. TimingSimpleCPU models memory access but is still in-order.

> The Execute stage includes the Functional Units (FU), which model the computational core of the CPU. By configuring the executeFuncUnits, as shown in Table 3, we can define functional units associated with our core. Each functional unit models a number of instruction classes, the delay between instruction issues, and the delay from instruction issue to commit [15]. Since Execute utilizes a Load Store Queue (LSQ) for memory reference instructions, there are some parameters that configure LSQ, as shown in Table 1.

This is interesting! I may not be working with a single instruction, but at least if the JIT can model these operations as a MMIO load/store, the FU could just write to MMIO & wait for done flag?

Think about interrupts. They might be useful for getting the result of computations. `src/arch/ARM/interrupts.hh`

FP register file… can be used for intermediate results, probably. Separate L1 Caches (Inst/Data), integrated L2 Cache. The HPI data cache includes a prefetcher which watches for access patterns.

HPI works! But is slow, esp for FS. don’t boot. Very slow. Upwards of 1 hr.

have to install gcc cross compiler. maybe just do LLVM if it doesn’t work, but trying to do local install. (can’t yum/apt-get on zoo) That was a little sketch – I don’t know if anything else needs to be installed or configured – but everything compiled fine!

benchmarks use ROI markers (region of interest). also reset m5 stats.

PARSEC instructions are out of date and confusing. Going to skip it. However, there is an interesting point about moving files onto the disk image. I think next step is to get cross-compile working.