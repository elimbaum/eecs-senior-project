# Senior Project Notes

 Eli Baum '19

Advisor: Rajit Manohar

## TODO

- [x] get set up on grace
- [x] github
- [ ] RHEL VM? https://software.yale.edu/software/red-hat-enterprise-linux
- [ ] install LLVM
- [x] reread DAISY
- [ ] read FPGA paper?
- [ ] read ADRES & other CGRA paper (dynamic translation!)
- [x] read routing paper
- [ ] look into how shared objects work, esp wrt BLAS
- [ ] object code translation
- [ ] GNU BFD
  - I don't know if this is feasible. We're not going to write a linker into the chip... and we don't want to require recompilation (or only partial compilation): this should be binary compatible.
- [ ] LZW for processor? replace based on usage, use code table to figure out common general operations
- [ ] profiling
- [ ] look at Wiki Dynamic Translation bib!
  - [ ] Dynamo
  - [ ] CGRA dynamic paper bib
- [ ] MorphoSys/TinyRISC: http://gram.eng.uci.edu/morphosys/

## Important Dates

12 Sep: Prospectus Due

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


## Notes

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

ADRES is effectively a coprocessor. transparent usage; many shared resources.

This requires a new compiler -- doesn't dynamically translate. There are many of these but this is not want I actually want to do.

### Dynamic CGRA

"Despite their potential benefit, CGRAs face a major adoption challenge as they do not execute a standard instruction stream." this is what I would like to fix!

DORA: Dynamic Optimization for Reconfigurable Architectures

this is good, I think this is just about what I'm trying to do.

Look at the existing dynamic binary translation efforts cited in this paper -- they may not have been particularly promising, but they may have interesting insights/guidance.

next-executing tail (NET) algorithm: find optimization candidates such as tight loops. NET works by identifying BACK branches, which are usually loops (cool!). If that code can be optimized (?) store PC in cache and substitute in optimized hardware.

*There is an optimization to NET, and more discussion of it w.r.t. Dynamo, on the AVLSI wiki bib.*

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