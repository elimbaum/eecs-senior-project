# Senior Project Update

Eli Baum

Advisor: Rajit Manohar

18 Dec 2018



At the beginning of the semester, I proposed an asynchronous CPU with run-time hardware optimization. If the system recognized software that matched a built-in hardware unit, that code could be replaced. I also proposed other optimizations, such as runtime instruction compression and bus remapping based on hardware usage. While I will keep these secondary goals in mind, my main focus will be on the base case of hardware functional unit optimization.

I spent most of this semester doing background research in the literature, as well as setting up and learning how to use the necessary compiler tools. Moving forward, I will develop an extension to the LLVM JIT (Just-in-time compiler) that will match incoming bitcode against available hardware units. A CPU using this system would inform the JIT of its hardware capabilities and emit the corresponding instruction if the equivalent software was called. For example, if a chip supported hardware FFT, and the JIT saw a call to a *software* FFT routine, it would instead emit an instruction to run the hardware unit.

The LLVM project has published a tutorial on building a compiler for a custom language which I am currently working through; although my project will not involve any such language (or any particular source language, for that matter; any LLVM frontend will be supported), I want to feel comfortable working with all aspects of the LLVM framework. From there, I will begin modifying the LLVM JIT to recognize a certain set of instructions in the BLAS Linear Algebra package. However, the final system need not be constrained to a particular library. It will read the set of supported functions off the chip itself, which will allow drop-in replacement on a wide variety of chips.

The most straightforward way to perform this optimization is simple pattern matching. Each function name would be mapped to a specific hardware unit: "SDOT" (the BLAS instruction which corresponds to a dot product) maps to function unit #1, etc. But a more advanced system could profile the code being run, and infer reasonable substitutions rather than being directly told them. In the above example, then, if the system saw two vectors being multiplied component-wise multiplied and then those products summed, it would infer a dot product was taking place (and call function unit #1). This development, of course, would be less generalizable, but might be feasible within specific domains (i.e. linear algebra).

Finally, I will implement one such functional unit in hardware (such as matrix-vector multiply), and simulate my system on a RISC platform. This will allow for a quantitative analysis of the sysetem's performance, which will inform its eventual efficacy. This system could eventually help chip designers tailor new architectures to specific applications without having to then take the time to reimplement compilers (and recompile code) for that new architecture.