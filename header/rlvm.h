#ifndef __RLVM_H__
#define __RLVM_H__

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

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

  extern int exec_bytecode (rlvm_t * vm, const size_t len, opcode_t * ops);

#ifdef __cplusplus
};
#endif /* !__cplusplus */

#endif /* !__RLVM_H__ */
