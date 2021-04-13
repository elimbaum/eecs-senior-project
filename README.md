# Dynamic Binary Translation for Optimized Functional Units (DBTOFU)

*This is my senior project for the B.S. degree in EECS. (Yale, 2019)
I have transferred this repository from the Yale Git, to which I no longer
have access.*

The project entailed writing an LLVM compiler pass to convert certain
function calls (in this case, common linear algebra routines) into calls
to specialized hardware. As written, this transformation happened at compile
time, but with the LLVM JIT, it could just as easily occur at runtime. The
idea was to allow custom hardware platforms to run common binaries, while
taking advantage of their specific hardware optimizations: you may have one
machine with fast matrix multiplication and another with fast matrix inversion;
DBTOFU would allow each machine to optimize the binary independently.

These initial results ran in simulation, using the `gem5` project. Additionally,
I used simple function-call matching for the transformation; more advanced schemes
could be imagined (i.e. code signatures of certain operations). The actual message
passing used memory-mapped IO (assigning specific function arguments to special
addresses).

In part beacuse I spent *so* long trying to wrap my head around LLVM, I didn't
manage to get to the point of showing improved resource usage (speed, energy, etc).
But this general scheme, I think, showed promise.

Important things in this repo:

- `blas`: various test programs/snippets for the BLAS linear algebra library
- `dbtofu final.pdf`: the final paper
- `mmiofu`: the actual simulator code + hardware approximations for the functional unit.
- `passes`: LLVM passes
- `sim`: simulation code
