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
 * This is used as both the return value of a VM instance and
 * also an exception construct. The field uid should only be
 * queried when the state is CLEAN or USER_DEFINED. In all
 * other cases, the uid should be zero although this is not
 * guaranteed.
 */
typedef struct status_t
{
  enum
  {
    CLEAN = 0, DIV_BY_ZERO, STACK_OFLOW, STACK_UFLOW, OUT_OF_MEM, USER_DEFINED
  } state;
  uint64_t uid;
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
  size_t stack_size;
  size_t sp;
  size_t ip;
  uint64_t iregs[ALLOC_REGS_COUNT];
  double fregs[ALLOC_REGS_COUNT];
  size_t *stack;
} rlvm_t;

#ifdef __cplusplus
extern "C"
{
#endif				/* !__cplusplus */

  extern rlvm_t init_rlvm (size_t stack_size);

  extern void print_rlvm_state (rlvm_t * vm);

  extern void clean_rlvm (rlvm_t * vm);

  extern status_t exec_bytecode (rlvm_t * vm, const size_t len,
				 opcode_t * ops);

#ifdef __cplusplus
};
#endif /* !__cplusplus */

#endif /* !__RLVM_H__ */
