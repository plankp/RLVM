/**
 * MIT License
 *
 * Copyright (c) 2016 Paul Teng
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __RLVM_H__
#define __RLVM_H__

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

/**
 * This is only used inside of the VM. On fault (or
 * exception), the stack unwinds until it reaches
 * the same location as its previous location
 * removing local variables allocated by another
 * function. The deallocation process does not
 * free the chunks on the heap.
 */
typedef struct ehandle_t
{
  uint64_t on_fault;
  uint64_t old_sp;
} ehandle_t;

/**
 * This is used as both the return value of a VM instance and
 * also an exception construct. The field uid should only be
 * queried when the state is CLEAN (return value), BAD_OPCODE
 * (the instruction that was illegal) and USER_DEFINED
 * (unique id). In all other cases, the uid should be zero
 * although this is not guaranteed.
 */
typedef union status_t
{
  struct
  {
    enum
    {
      CLEAN = 0, DIV_BY_ZERO, STACK_OFLOW, STACK_UFLOW, OUT_OF_MEM,
      BAD_OPCODE, USER_DEFINED
    } state:4;			/* Making it 4 bits for future states */
    uint64_t uid:60;		/* This should be enough */
  };
  uint64_t bytes;
} status_t;

/*
 * The opcode layout is very similar to the MIPS ISA
 *
 *31                                    0
 * 000000 00000 00000 00000 00000 000000
 * | op  |  rs |  rt |  rd |  sa |  fn | op_fvar_t
 * | op  |  rs |  rt |    immediate    | op_svar_t
 * | op  |            target           | op_tvar_t
 */
typedef struct op_fvar_t
{
  unsigned int opcode:6;
  unsigned int rs:5;
  unsigned int rt:5;
  unsigned int rd:5;
  unsigned int sa:5;
  unsigned int fn:6;
} op_fvar_t;

typedef struct op_svar_t
{
  unsigned int opcode:6;
  unsigned int rs:5;
  unsigned int rt:5;
  unsigned int immediate:16;
} op_svar_t;

typedef struct op_tvar_t
{
  unsigned int opcode:6;
  unsigned int target:26;
} op_tvar_t;

typedef union opcode_t
{
  op_fvar_t fvar;
  op_svar_t svar;
  op_tvar_t tvar;
  uint32_t bytes;
} opcode_t;

/*
 * ISA restricts the amount of registers to 32. Since the float points
 * and the ints (maybe vector types) use different registers, we will
 * have 32 uint64_t's and 32 doubles (which according to IEEE, it is
 * 64-bits)
 *
 * For now, we ignore the vector type
 */
#define ALLOC_REGS_COUNT 32

typedef struct rlvm_t
{
  uint64_t stack_size;		/* Call stack size */
  uint64_t handler_size;	/* Exception handler stack size */
  uint64_t sp;			/* Call stack pointer */
  uint64_t ip;			/* Instruction pointer */
  uint64_t esp;			/* Exception stack pointer */
  status_t state;		/* VM state, also stores latest exception */
  uint64_t iregs[ALLOC_REGS_COUNT];	/* Integer registers */
  double fregs[ALLOC_REGS_COUNT];	/* Float point registers */
  uint64_t *stack;		/* Call stack */
  ehandle_t *estack;		/* Exception stack */
} rlvm_t;

#ifdef __cplusplus
extern "C"
{
#endif				/* !__cplusplus */

  extern rlvm_t init_rlvm (uint64_t stack_size, uint64_t handler_size);

  extern void print_rlvm_state (rlvm_t * vm);

  extern void clean_rlvm (rlvm_t * vm);

  extern status_t exec_bytecode (rlvm_t * vm, const uint64_t len,
				 opcode_t * ops);

#ifdef __cplusplus
};
#endif /* !__cplusplus */

#endif /* !__RLVM_H__ */
