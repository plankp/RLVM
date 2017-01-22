# RLVM

A RISC inspired, Not sure about the L, Virtual Machine.

Currently there is no assembler, so if you do feel adventurous,
either write some C (or C++) code and pipe out the bytecode or hand compile it!

The full list of instructions can be found in `header/bcode.h`.
They are prefixed with `RLVM_` and are macros to make code generation easier.

To run the pre-assembled bytecode `sample/gcd.bin`, enter this command

```
rlvm sample/gcd.bin
```

Built using CMake and released under the MIT license.
