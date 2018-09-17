# Senior Project Notes

 Eli Baum '19

Advisor: Rajit Manohar

## TODO

- [ ] get set up on grace
- [ ] install LLVM
- [ ] github

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


## Papers

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

running a firmware VM, essentially. Will cache previous translations.