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

#ifndef __BCODE_H__
#define __BCODE_H__

/* Feature test macros (or something like that) */
#define _DEFAULT_SOURCE

#include "rlvm.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <endian.h>

/**
 * Representation of a RLVM bytecode file. For the magic bytes, the
 * first byte must be 0x2C. The second byte depends of the endianess.
 * For big endian, it would be 0xDF and 0xD0 for little endian. The
 * endianess determines how the fields would be represented; in other
 * words, the endianess of the binary file.
 */

typedef struct bcode_t
{
  uint8_t magic[2];
  uint64_t cstack_size;
  uint64_t estack_size;
  uint64_t code_size;
  opcode_t *code;
} bcode_t;

/**
 * Halt instance with ireg as return value
 */
#define RLVM_HALT(ireg)				\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ireg,				\
      .rt = 0,					\
      .rd = 0,					\
      .sa = 0,					\
      .fn = 0					\
    }						\
  }

/**
 * Move between int registers
 */
#define RLVM_IRMV64(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 0,					\
      .fn = 1					\
    }						\
  }

/**
 * Move the upper 32 bits between int registers
 */
#define RLVM_IRMVH32(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 1,					\
      .fn = 1					\
    }						\
  }

/**
 * Move the lower 32 bits between int registers
 */
#define RLVM_IRMVL32(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 2,					\
      .fn = 1					\
    }						\
  }

/**
 * Move the lower 16 bits between int registers
 */
#define RLVM_IRMVL16(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 3,					\
      .fn = 1					\
    }						\
  }

/**
 * Move the lower 8 bits between int registers
 */
#define RLVM_IRMVL8(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 4,					\
      .fn = 1					\
    }						\
  }

/**
 * Move between float registers
 */
#define RLVM_FRMV(frDst, frSrc)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = frSrc,				\
      .rt = 0,					\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 2					\
    }						\
  }

/**
 * Swap between int registers
 */
#define RLVM_IRSWP(irDst, irSrc)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = 0,					\
      .rd = irDst,				\
      .sa = 0,					\
      .fn = 3					\
    }						\
  }

/**
 * Move from int register to float register
 */
#define RLVM_IRTFR(frDst, irSrc, mode)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = irSrc,				\
      .rt = mode,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 4					\
    }						\
  }

/**
 * Move from float register to int register
 */
#define RLVM_FRTIR(irDst, frSrc, mode)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = frSrc,				\
      .rt = mode,				\
      .rd = irDst,				\
      .sa = 0,					\
      .fn = 5					\
    }						\
  }

/**
 * Removes exception handler from exception stack
 */
#define RLVM_RMEH()				\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = 0,					\
      .rt = 0,					\
      .rd = 0,					\
      .sa = 0,					\
      .fn = 6					\
    }						\
  }

/**
 * Throws exception
 */
#define RLVM_THROW(ireg)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ireg,				\
      .rt = 0,					\
      .rd = 0,					\
      .sa = 0,					\
      .fn = 7					\
    }						\
  }

/**
 * Push int registers
 */
#define RLVM_PUSH1(ireg)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ireg,				\
      .rt = 0,					\
      .rd = 0,					\
      .sa = 0,					\
      .fn = 8					\
    }						\
  }

#define RLVM_PUSH2(ir1, ir2)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .rd = 0,					\
      .sa = 1,					\
      .fn = 8					\
    }						\
  }

#define RLVM_PUSH3(ir1, ir2, ir3)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .rd = ir3,				\
      .sa = 2,					\
      .fn = 8					\
    }						\
  }

/**
 * Pop int registers
 */
#define RLVM_POP1(ireg)				\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ireg,				\
      .rt = 0,					\
      .rd = 0,					\
      .sa = 4,					\
      .fn = 8					\
    }						\
  }

#define RLVM_POP2(ir1, ir2)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .rd = 0,					\
      .sa = 5,					\
      .fn = 8					\
    }						\
  }

#define RLVM_POP3(ir1, ir2, ir3)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .rd = ir3,				\
      .sa = 6,					\
      .fn = 8					\
    }						\
  }

/**
 * Loads virtual machine state (or exception)
 */
#define RLVM_LDEX(ireg, mode)			\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 0,				\
      .rs = 0,					\
      .rt = 0,					\
      .rd = ireg,				\
      .sa = mode,				\
      .fn = 9					\
    }						\
  }

/**
 * int registers related math
 */
#define RLVM_ADD(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 0				\
    }							\
  }

#define RLVM_SUB(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 1				\
    }							\
  }

#define RLVM_MUL(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 2				\
    }							\
  }

#define RLVM_DIV(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 3				\
    }							\
  }

#define RLVM_MOD(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 4				\
    }							\
  }

#define RLVM_AND(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 5				\
    }							\
  }

#define RLVM_OR(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 6				\
    }							\
  }

#define RLVM_XOR(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 7				\
    }							\
  }

#define RLVM_NOT(irDst, irRhs, shift, count)		\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = 0,						\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 8				\
    }							\
  }

#define RLVM_LSH(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 9				\
    }							\
  }

#define RLVM_RSH(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 10				\
    }							\
  }

#define RLVM_SRHS(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 11				\
    }							\
  }

#define RLVM_ROL(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 12				\
    }							\
  }

#define RLVM_ROR(irDst, irLhs, irRhs, shift, count)	\
  (opcode_t) {						\
    .fvar = (op_fvar_t) {				\
      .opcode = 1,					\
      .rs = irLhs,					\
      .rt = irRhs,					\
      .rd = irDst,					\
      .sa = count,					\
      .fn = shift << 4 | 13				\
    }							\
  }

/**
 * float registers related math
 */
#define RLVM_ADDF(frDst, frLhs, frRhs)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 2,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 0					\
    }						\
  }

#define RLVM_SUBF(frDst, frLhs, frRhs)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 2,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 1					\
    }						\
  }

#define RLVM_MULF(frDst, frLhs, frRhs)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 2,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 2					\
    }						\
  }

#define RLVM_DIVF(frDst, frLhs, frRhs)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 2,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 3					\
    }						\
  }

#define RLVM_MODF(frDst, frLhs, frRhs)		\
  (opcode_t) {					\
    .fvar = (op_fvar_t) {			\
      .opcode = 2,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .rd = frDst,				\
      .sa = 0,					\
      .fn = 4					\
    }						\
  }

/**
 * Load integer immediate to int register
 */
#define RLVM_IRLDI(irDst, lshCount, imm)	\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 3,				\
      .rs = irDst,				\
      .rt = lshCount,				\
      .immediate = imm				\
    }						\
  }

/**
 * Add integer immediate to int register
 */
#define RLVM_ADDI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 4,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Subtract integer immediate from int register
 */
#define RLVM_SUBI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 5,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Multiply integer immediate from int register
 */
#define RLVM_MULI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 6,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Divide integer immediate from int register
 */
#define RLVM_DIVI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 7,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Remainder of integer immediate and int register
 */
#define RLVM_MODI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 8,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Bitwise and of integer immediate and int register
 */
#define RLVM_ANDI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 9,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Bitwise or of integer immediate and int register
 */
#define RLVM_ORI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 10,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Bitwise xor of integer immediate and int register
 */
#define RLVM_XORI(irDst, irLhs, imm)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 11,				\
      .rs = irDst,				\
      .rt = irLhs,				\
      .immediate = imm				\
    }						\
  }

/**
 * Calls a code address
 */
#define RLVM_CALL(addr)				\
  (opcode_t) {					\
    .tvar = (op_tvar_t) {			\
      .opcode = 12,				\
      .target = addr				\
    }						\
  }

/**
 * Jumps to a code address
 */
#define RLVM_JMP(addr)				\
  (opcode_t) {					\
    .tvar = (op_tvar_t) {			\
      .opcode = 13,				\
      .target = addr				\
    }						\
  }

/**
 * Returns from a subroutine. Return value is
 * passed through the stack!
 */
#define RLVM_RET()				\
  (opcode_t) {					\
    .tvar = (op_tvar_t) {			\
      .opcode = 14,				\
      .target = 0,				\
    }						\
  }

/**
 * Jump if int registers equal
 */
#define RLVM_JE(irLhs, irRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 15,				\
      .rs = irLhs,				\
      .rt = irRhs,				\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if int registers are less
 */
#define RLVM_JL(irLhs, irRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 16,				\
      .rs = irLhs,				\
      .rt = irRhs,				\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if int registers are greater
 */
#define RLVM_JG(irLhs, irRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 17,				\
      .rs = irLhs,				\
      .rt = irRhs,				\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if signed int registers are less
 */
#define RLVM_JLS(irLhs, irRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 18,				\
      .rs = irLhs,				\
      .rt = irRhs,				\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if signed int registers are greater
 */
#define RLVM_JGS(irLhs, irRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 19,				\
      .rs = irLhs,				\
      .rt = irRhs,				\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if float registers equal
 */
#define RLVM_JFE(frLhs, frRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 20,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .immediate = addr,			\
    }						\
  }

/**
 * Jump if float registers are less
 */
#define RLVM_JFL(frLhs, frRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 21,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .immediate = addr,			\
    }						\
  }

/**
 * Jump if float registers are greater
 */
#define RLVM_JFG(frLhs, frRhs, addr)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 22,				\
      .rs = frLhs,				\
      .rt = frRhs,				\
      .immediate = addr,			\
    }						\
  }

/**
 * Jumps to an offset
 */
#define RLVM_JOF(addr)				\
  (opcode_t) {					\
    .tvar = (op_tvar_t) {			\
      .opcode = 23,				\
      .target = addr				\
    }						\
  }

/**
 * Jump to int register
 */
#define RLVM_JIR(irVal, irLhs, immOff)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 24,				\
      .rs = irVal,				\
      .rt = irLhs,				\
      .immediate = immOff,			\
    }						\
  }

/**
 * Jump if float register is zero
 */
#define RLVM_JFRZ(freg, addr)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 25,				\
      .rs = freg,				\
      .rt = 1,					\
      .immediate = addr				\
    }						\
  }

/**
 * Jump if int register is zero
 */
#define RLVM_JIRZ(ireg, addr)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 25,				\
      .rs = ireg,				\
      .rt = 0,					\
      .immediate = addr				\
    }						\
  }

/**
 * Install new exception handler
 */
#define RLVM_INEH(addr)				\
  (opcode_t) {					\
    .tvar = (op_tvar_t) {			\
      .opcode = 26,				\
      .target = addr				\
    }						\
  }

/**
 * Loads from stack to int register
 */
#define RLVM_IRLD(ireg, offset)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 27,				\
      .rs = 0,					\
      .rt = ireg,				\
      .immediate = offset			\
    }						\
  }

/**
 * Load bits from stack to float register
 */
#define RLVM_FRLD(freg, offset)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 27,				\
      .rs = 1,					\
      .rt = freg,				\
      .immediate = offset			\
    }						\
  }

/**
 * Store int register to stack
 */
#define RLVM_IRST(ireg, offset)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 27,				\
      .rs = 2,					\
      .rt = ireg,				\
      .immediate = offset			\
    }						\
  }

/**
 * Store float register to stack
 */
#define RLVM_FRST(freg, offset)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 27,				\
      .rs = 3,					\
      .rt = freg,				\
      .immediate = offset			\
    }						\
  }

/**
 * Store float register bits to stack
 */
#define RLVM_FBST(freg, offset)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 27,				\
      .rs = 4,					\
      .rt = freg,				\
      .immediate = offset			\
    }						\
  }

/**
 * Allocate bytes on the heap
 */
#define RLVM_ALLOCI(ireg, size)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 28,				\
      .rs = 0,					\
      .rt = ireg,				\
      .immediate = size				\
    }						\
  }

#define RLVM_IRALLOC(ireg, irSize)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 28,				\
      .rs = irSize,				\
      .rt = ireg,				\
      .immediate = 0				\
    }						\
  }

/**
 * Free bytes on the heap
 */
#define RLVM_FREE(ireg)				\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 29,				\
      .rs = 0,					\
      .rt = ireg,				\
      .immediate = 0,				\
    }						\
  }

/**
 * Loads a byte (8 bit) on the heap
 */
#define RLVM_LDB(irDst, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 30,				\
      .rs = irBase,				\
      .rt = irDst,				\
      .immediate = offset			\
    }						\
  }

/**
 * Loads a word (16 bit) on the heap
 */
#define RLVM_LDW(irDst, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 31,				\
      .rs = irBase,				\
      .rt = irDst,				\
      .immediate = offset			\
    }						\
  }

/**
 * Loads a double (32 bit) on the heap
 */
#define RLVM_LDD(irDst, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 32,				\
      .rs = irBase,				\
      .rt = irDst,				\
      .immediate = offset			\
    }						\
  }

/**
 * Loads a quad (64 bit) on the heap
 */
#define RLVM_LDQ(irDst, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 33,				\
      .rs = irBase,				\
      .rt = irDst,				\
      .immediate = offset			\
    }						\
  }

/**
 * Stores a byte (8 bit) on the heap
 */
#define RLVM_STB(irSrc, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 34,				\
      .rs = irBase,				\
      .rt = irSrc,				\
      .immediate = offset			\
    }						\
  }

/**
 * Stores a word (16 bit) on the heap
 */
#define RLVM_STW(irSrc, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 35,				\
      .rs = irBase,				\
      .rt = irSrc,				\
      .immediate = offset			\
    }						\
  }

/**
 * Stores a double (32 bit) on the heap
 */
#define RLVM_STD(irSrc, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 36,				\
      .rs = irBase,				\
      .rt = irSrc,				\
      .immediate = offset			\
    }						\
  }

/**
 * Stores a quad (64 bit) on the heap
 */
#define RLVM_STQ(irSrc, irBase, offset)		\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 37,				\
      .rs = irBase,				\
      .rt = irSrc,				\
      .immediate = offset			\
    }						\
  }

/**
 * Skip compare: int registers are equal
 */
#define RLVM_SIREQ(ir1, ir2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .immediate = 0				\
    }						\
  }

/**
 * Skip compare: float registers are equal
 */
#define RLVM_SFREQ(fr1, fr2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = fr1,				\
      .rt = fr2,				\
      .immediate = 0 | 8			\
    }						\
  }

/**
 * Skip compare: int registers are less
 */
#define RLVM_SIRL(ir1, ir2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .immediate = 1				\
    }						\
  }

/**
 * Skip compare: signed int registers are less
 */
#define RLVM_SIRSL(ir1, ir2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .immediate = 1 | 4			\
    }						\
  }

/**
 * Skip compare: float registers are less
 */
#define RLVM_SFRL(fr1, fr2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = fr1,				\
      .rt = fr2,				\
      .immediate = 1 | 8			\
    }						\
  }

/**
 * Skip compare: int registers are more
 */
#define RLVM_SIRG(ir1, ir2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .immediate = 2				\
    }						\
  }

/**
 * Skip compare: signed int registers are more
 */
#define RLVM_SIRSG(ir1, ir2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ir1,				\
      .rt = ir2,				\
      .immediate = 2 | 4			\
    }						\
  }

/**
 * Skip compare: float registers are more
 */
#define RLVM_SFRG(fr1, fr2)			\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = fr1,				\
      .rt = fr2,				\
      .immediate = 2 | 8			\
    }						\
  }

/**
 * Skip compare: int register is zero
 */
#define RLVM_SIRZ(ireg)				\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = ireg,				\
      .rt = 0,					\
      .immediate = 3				\
    }						\
  }

/**
 * Skip compare: float register is zero
 */
#define RLVM_SFRZ(freg)				\
  (opcode_t) {					\
    .svar = (op_svar_t) {			\
      .opcode = 38,				\
      .rs = freg,				\
      .rt = 0,					\
      .immediate = 3 | 8			\
    }						\
  }

#ifdef __cplusplus
extern "C"
{
#endif				/* !__cplusplus */

  extern bcode_t *read_bytecode (FILE * f, bcode_t * bf);

  extern bool write_bytecode (FILE * f, bcode_t * bf);

  extern status_t exec_bcode_t (rlvm_t * vm, bcode_t * bf);

  extern void clean_bcode (bcode_t * bf);

#ifdef __cplusplus
};
#endif /* !__cplusplus */

#endif /* !__BCODE_H__ */
