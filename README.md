# RLVM

A RISC inspired, Not sure about the L, Virtual Machine.

Current ways of generating RLVM bytecode:

1.  Write RLVM assembly
2.  Construct with C (or C++) code
3.  Hand compile with hex editor

The full list of instructions can be found in `header/bcode.h`.
They are prefixed with `RLVM_` and are macros to make code generation easier.

To run the pre-assembled bytecode `sample/gcd.bin`, enter this command

```
rlvm -r sample/gcd.bin
```

Of course, the rlvm binary also comes with an assembler.
To assemble rlvm assembly and then run it directly, enter this command

```
rlvm -cr your/file.asm
```

To assemble rlvm assembly and then save it, enter this command

```
rlvm -c -o output/name.bin your/file.asm
```

Note: `your/file.asm` has to be the at the end

To get a `objdump` like output, enter this command

```
rlvm -d some/binary.bin
```

To get help, type

```
rlvm -h
```

Built using CMake and released under the MIT license.
