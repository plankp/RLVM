# List of instructions

`r%d` indicates any integer register (0 to 31)

`fp%d` indicates any float point register (0 to 31)

`#` indicates a numerical literal (or number)

`<text>` indicates any label declared in the text (code) section

`<data>` indicates any label declared in the data section

`$n` indicates the parameter. First index is 1.

| Instruction                 | Meaning |
|-----------------------------|---------|
| HALT r%d                    | Halts the program with exit code of `$1` |
| MOV r%d, r%d                | Moves `$2` to `$1` |
| MOV fp%d, fp%d              | Moves `$2` to `$1` |
| MOV r%d, #                  | Moves `$2` to `$1` |
| MOV r%d, #, LSH #           | Moves `$2 << $3` to `$1` |
| MH32 r%d, r%d               | Moves the upper 32 bits of `$2` to `$1` |
| ML32 r%d, r%d               | Moves the lower 32 bits of `$2` to `$1` |
| ML16 r%d, r%d               | Moves the lower 16 bits of `$2` to `$1` |
| ML8 r%d, r%d                | Moves the lower 8 bits of `$2` to `$1` |
| SWP r%d, r%d                | Swaps the values of `$2` and `$1` |
| I2F fp%d, r%d               | Moves textual value of `$2` to `$1` |
| B2F fp%d, r%d               | Performs a `reinterpret_cast` of `$2` to `$1` |
| F2IF r%d, fp%d              | Moves floor value of `$2` to `$1` |
| F2B fp%d, r%d               | Performs a `reinterpret_cast` of `$2` to `$1` |
| F2IC r%d, fp%d              | Moves ceiling value of `$2` to `$1` |
| RMEH                        | Pops the exception handler |
| THROW r%d                   | Throws an exception with value of `$1` |
| PUSH r%d                    | Pushes `$1` onto the stack |
| PUSH r%d, r%d               | Pushes `$1` and `$2` onto the stack |
| PUSH r%d, r%d, r%d          | Pushes `$1`, `$2`, and `$3` onto the stack |
| POP r%d                     | Pops `$1` from the stack |
| POP r%d, r%d                | Pops `$1` and `$2` from the stack |
| POP r%d, r%d, r%d           | Pops `$1`, `$2`, and `$3` from the stack |
| LDEX                        | Pushes the most recently caught exception onto the stack |
| LDEX r%d                    | Moves the most recently caught exception to `$1` |
| PLDEX r%d                   | Push and moves (to `$1`) the most recently caught exception |
| ADD r%d, r%d, r%d           | `$1 = $2 + $3` |
| ADD r%d, r%d, r%d, LSH #    | `$1 = $2 + ($3 << $4)` |
| ADD r%d, r%d, r%d, RSH #    | `$1 = $2 + ($3 >>> $4)` |
| ADD r%d, r%d, r%d, SRSH #   | `$1 = $2 + ($3 >> $4)` |
| ADD fp%d, fp%d, fp%d        | `$1 = $2 + $3` |
| ADD r%d, r%d, #             | `$1 = $2 + $3` |
| SUB r%d, r%d, r%d           | `$1 = $2 - $3` |
| SUB r%d, r%d, r%d, LSH #    | `$1 = $2 - ($3 << $4)` |
| SUB r%d, r%d, r%d, RSH #    | `$1 = $2 - ($3 >>> $4)` |
| SUB r%d, r%d, r%d, SRSH #   | `$1 = $2 - ($3 >> $4)` |
| SUB fp%d, fp%d, fp%d        | `$1 = $2 - $3` |
| SUB r%d, r%d, #             | `$1 = $2 - $3` |
| MUL r%d, r%d, r%d           | `$1 = $2 * $3` |
| MUL r%d, r%d, r%d, LSH #    | `$1 = $2 * ($3 << $4)` |
| MUL r%d, r%d, r%d, RSH #    | `$1 = $2 * ($3 >>> $4)` |
| MUL r%d, r%d, r%d, SRSH #   | `$1 = $2 * ($3 >> $4)` |
| MUL fp%d, fp%d, fp%d        | `$1 = $2 * $3` |
| MUL r%d, r%d, #             | `$1 = $2 * $3` |
| DIV r%d, r%d, r%d           | `$1 = $2 / $3` |
| DIV r%d, r%d, r%d, LSH #    | `$1 = $2 / ($3 << $4)` |
| DIV r%d, r%d, r%d, RSH #    | `$1 = $2 / ($3 >>> $4)` |
| DIV r%d, r%d, r%d, SRSH #   | `$1 = $2 / ($3 >> $4)` |
| DIV fp%d, fp%d, fp%d        | `$1 = $2 / $3` |
| DIV r%d, r%d, #             | `$1 = $2 / $3` |
| MOD r%d, r%d, r%d           | `$1 = $2 % $3` |
| MOD r%d, r%d, r%d, LSH #    | `$1 = $2 % ($3 << $4)` |
| MOD r%d, r%d, r%d, RSH #    | `$1 = $2 % ($3 >>> $4)` |
| MOD r%d, r%d, r%d, SRSH #   | `$1 = $2 % ($3 >> $4)` |
| MOD fp%d, fp%d, fp%d        | `$1 = $2 % $3` |
| MOD r%d, r%d, #             | `$1 = $2 % $3` |
| AND r%d, r%d, r%d           | `$1 = $2 & $3` |
| AND r%d, r%d, r%d, LSH #    | `$1 = $2 & ($3 << $4)` |
| AND r%d, r%d, r%d, RSH #    | `$1 = $2 & ($3 >>> $4)` |
| AND r%d, r%d, r%d, SRSH #   | `$1 = $2 & ($3 >> $4)` |
| AND r%d, r%d, #             | `$1 = $2 & $3` |
| OR r%d, r%d, r%d            | `$1 = $2 | $3` |
| OR r%d, r%d, r%d, LSH #     | `$1 = $2 | ($3 << $4)` |
| OR r%d, r%d, r%d, RSH #     | `$1 = $2 | ($3 >>> $4)` |
| OR r%d, r%d, r%d, SRSH #    | `$1 = $2 | ($3 >> $4)` |
| OR r%d, r%d, #              | `$1 = $2 | $3` |
| XOR r%d, r%d, r%d           | `$1 = $2 ^ $3` |
| XOR r%d, r%d, r%d, LSH #    | `$1 = $2 ^ ($3 << $4)` |
| XOR r%d, r%d, r%d, RSH #    | `$1 = $2 ^ ($3 >>> $4)` |
| XOR r%d, r%d, r%d, SRSH #   | `$1 = $2 ^ ($3 >> $4)` |
| XOR r%d, r%d, #             | `$1 = $2 ^ $3` |
| NOT r%d, r%d                | `$1 = ~$2` |
| NOT r%d, r%d, LSH #         | `$1 = ~($2 << $4)` |
| NOT r%d, r%d, RSH #         | `$1 = ~($2 >>> $4)` |
| NOT r%d, r%d, SRSH #        | `$1 = ~($2 >> $4)` |
| LSH r%d, r%d, r%d           | `$1 = $2 << $3` |
| LSH r%d, r%d, r%d, LSH #    | `$1 = $2 << ($3 << $4)` |
| LSH r%d, r%d, r%d, RSH #    | `$1 = $2 << ($3 >>> $4)` |
| LSH r%d, r%d, r%d, SRSH #   | `$1 = $2 << ($3 >> $4)` |
| RSH r%d, r%d, r%d           | `$1 = $2 >>> $3` |
| RSH r%d, r%d, r%d, LSH #    | `$1 = $2 >>> ($3 << $4)` |
| RSH r%d, r%d, r%d, RSH #    | `$1 = $2 >>> ($3 >>> $4)` |
| RSH r%d, r%d, r%d, SRSH #   | `$1 = $2 >>> ($3 >> $4)` |
| SRSH r%d, r%d, r%d          | `$1 = $2 >> $3` |
| SRSH r%d, r%d, r%d, LSH #   | `$1 = $2 >> ($3 << $4)` |
| SRSH r%d, r%d, r%d, RSH #   | `$1 = $2 >> ($3 >>> $4)` |
| SRSH r%d, r%d, r%d, SRSH #  | `$1 = $2 >> ($3 >> $4)` |
| ROL r%d, r%d, r%d           | `$1 = $2 rotate-left $3` |
| ROL r%d, r%d, r%d, LSH #    | `$1 = $2 rotate-left ($3 << $4)` |
| ROL r%d, r%d, r%d, RSH #    | `$1 = $2 rotate-left ($3 >>> $4)` |
| ROL r%d, r%d, r%d, SRSH #   | `$1 = $2 rotate-left ($3 >> $4)` |
| ROR r%d, r%d, r%d           | `$1 = $2 rotate-right $3` |
| ROR r%d, r%d, r%d, LSH #    | `$1 = $2 rotate-right ($3 << $4)` |
| ROR r%d, r%d, r%d, RSH #    | `$1 = $2 rotate-right ($3 >>> $4)` |
| ROR r%d, r%d, r%d, SRSH #   | `$1 = $2 rotate-right ($3 >> $4)` |
| CALL &lt;text&gt;           | Jumps and pushes return address onto the stack |
| JUMP &lt;text&gt;           | Jumps to the label |
| RET                         | Pops return address from the stack and jumps to it |
| JE r%d, r%d, &lt;text&gt;   | Jumps if `$1 == $2` |
| JL r%d, r%d, &lt;text&gt;   | Jumps if `$1 < $2` |
| JG r%d, r%d, &lt;text&gt;   | Jumps if `$1 > $2` |
| JLS r%d, r%d, &lt;text&gt;  | Jumps if `(signed) $1 < (signed) $2` |
| JGS r%d, r%d, &lt;text&gt;  | Jumps if `(signed) $1 > (signed) $2` |
| JE fp%d, fp%d, &lt;text&gt; | Jumps if `$1 == $2` |
| JL fp%d, fp%d, &lt;text&gt; | Jumps if `$1 < $2` |
| JG fp%d, fp%d, &lt;text&gt; | Jumps if `$1 > $2` |
| JOF #                       | Jumps to a signed offset of `$1` |
| JMP r%d                     | Jumps to address of `$1` |
| JZ r%d, &lt;text&gt;        | Jumps if `$1` is zero |
| JZ fp%d, &lt;text&gt;       | Jumps if `$1` is zero |
| INEH &lt;text&gt;           | Adds a handler that jumps to `$1` on exception |
| LDS r%d, #                  | Loads value on the stack with offset of `$2` to `$1` |
| LDS fp%d, #                 | Loads value on the stack with offset of `$2` to `$1` |
| STS r%d, #                  | Stores `$1` onto the stack with offset of `$2` |
| STS fp%d, #                 | Stores textual value of `$1` onto the stack with offset of `$2` |
| STFBS fp%d, #               | Stores reinterpreted value of `$1` onto the stack with offset of `$2` |
| ALLOC r%d, #                | Allocates `$2` amount of bytes and stores the pointer into `$1` |
| ALLOC r%d, r%d              | Allocates `$2` amount of bytes and stores the pointer into `$1` |
| FREE r%d                    | Frees the pointer of `$1` |
| LDB r%d, r%d, #             | Loads value at `$2 + (signed) $3` to `$1` with `$3` being 8 bits |
| LDW r%d, r%d, #             | Loads value at `$2 + (signed) $3` to `$1` with `$3` being 16 bits |
| LDD r%d, r%d, #             | Loads value at `$2 + (signed) $3` to `$1` with `$3` being 32 bits |
| LDQ r%d, r%d, #             | Loads value at `$2 + (signed) $3` to `$1` with `$3` being 64 bits |
| STB r%d, r%d, #             | Stores value of `$1` to `$2 + (signed) $3` with `$3` being 8 bits |
| STW r%d, r%d, #             | Stores value of `$1` to `$2 + (signed) $3` with `$3` being 16 bits |
| STD r%d, r%d, #             | Stores value of `$1` to `$2 + (signed) $3` with `$3` being 32 bits |
| STQ r%d, r%d, #             | Stores value of `$1` to `$2 + (signed) $3` with `$3` being 64 bits |
| SJE r%d, r%d                | Executes next line if `$1 == $2`, skips otherwise. |
| SJE fp%d, fp%d              | Executes next line if `$1 == $2`, skips otherwise. |
| SJL r%d, r%d                | Executes next line if `$1 < $2`, skips otherwise. |
| SJL fp%d, fp%d              | Executes next line if `$1 < $2`, skips otherwise. |
| SJG r%d, r%d                | Executes next line if `$1 > $2`, skips otherwise. |
| SJG fp%d, fp%d              | Executes next line if `$1 > $2`, skips otherwise. |
| SJSL r%d, r%d               | Executes next line if `(signed) $1 < (signed) $2`, skips otherwise. |
| SJSG r%d, r%d               | Executes next line if `(signed) $1 > (signed) $2`, skips otherwise. |
| SJZ r%d                     | Executes next line if `$1` is zero, skips otherwise |
| SJZ fp%d                    | Executes next line if `$1` is zero, skips otherwise |
| LDC r%d, r%d, #             | Loads the address of `$2` with the offset (in 8 bits) of `$3` to `$1` |
| LDC r%d, &lt;data&gt;       | Loads the address of `$2` to `$1` |
| FREAD r%d, r%d              | Reads a `char` to `$1` from `$2` where `$2` is a `FILE*` |
| FREAD r%d, r%d, r%d         | Reads a `int64` to `$1` from `$2` where `$2` is a `FILE*`. Return value is stored at `$3`. |
| FREAD fp%d, r%d, r%d        | Reads a `double` to `$1` from `$2` where `$2` is a `FILE*`. Return value is stored at `$3`. |
| LDC r%d, STDOUT             | Stores the `FILE*` mapped to the console output to `$1` |
| LDC r%d, STDERR             | Stores the `FILE*` mapped to the console output to `$1` |
| LDC r%d, STDIN              | Stores the `FILE*` mapped to the console output to `$1` |
| FOPEN r%d, r%d, r%d         | Stores the `FILE*` to `$1` with `$2` being a pointer to a file name (null terminated) and `$3` being a pointer to the open mode (also null terminated) |
| FCLOSE r%d, r%d             | Closes `$2` where `$2` is a `FILE*`. Return value is stored at `$1`. |
| FWRTB r%d, r%d, r%d         | Writes `$3` as a `char` to `$2` with `$2` being a `FILE*`. Return value is stored at `$1`. |
| FWRTQ r%d, r%d, r%d         | Writes `$3` as a `int64` to `$2` with `$2` being a `FILE*`. Return value is stored at `$1`. |
| FWRTQ r%d, r%d, fp%d        | Writes `$3` as a `double` to `$2` with `$2` being a `FILE*`. Return value is stored at `$1`. |
| FWRTS r%d, r%d, r%d         | Writes `$3` (a pointer to a null terminated string) to `$2` with `$2` being a `FILE*`. Return value is stored at `$1`. |
