# Senior Project Prospectus

Eli Baum 2019, BS in EECS

Advisor: Rajit Manohar

11 September 2018



Engineers design modern CPUs around worst-case execution scenarios. The critical path through a processor governs the speed of the entire chip; fast instructions are forced to wait for slower instructions. Furthermore, common instructions use just as much instruction memory as uncommon ones. Asynchronous circuits largely solve the timing issue by optimizing average-case behavior, because execution is not tied to a global clock and instructions can execute immediately after prior ones finish.

Benes et al. ([10.1109/ASYNC.1998.666493](http://doi.org/10.1109/ASYNC.1998.666493)) developed a memory optimization that Huffman-encodes instructions during compilation, stores them in compressed form, and only decodes these instructions when loading them into cache. This design is much faster, yet significantly smaller, than existing instruction compression systems. Ebcioglu & Altman ([10.1145/384286.264126](https://doi.org/10.1145/384286.264126)) designed a firmware just-in-time compiler (JIT) to dynamically emulate different instruction set architectures. They demonstrate the concept by converting PowerPC instructions on cache load into parallelized very long instruction words (VLIW). However, this process could be used to translate between any instruction sets at runtime.

I propose a new design for an asynchronous CPU that will attempt to build upon the Huffman coding of Benes et al. and Ebcioglu & Altman's JIT. Not only will this system compress instructions at compile-time, but it will also optimize entire functional units while code is running. If the system recognizes a certain instruction sequence which matches an available hardware functional unit (for example, a matrix multiplier), it will instead issue a call to this optimized unit. The chip will also move frequently used units  into more-accesible bus positions, similarly to how an FPGA restructures functional blocks to perform arbitrary operations. In total, this new system should offer higher memory efficiency together with faster execution, while not requiring software to be rewritten, or even recompiled, to take advantage of hardware optimizations. The bulk of this project will consist of designing, simulating, and testing the experimental parts of this processor.

