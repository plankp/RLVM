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

#include "rlvm.h"

/*
 * Using union to reinterpret_cast between a 64 bit integer and
 * a double (which according to the IEEE, it is 64-bits)
 */
union fp_i_conv_t
{
  uint64_t ival;
  double fval;
};

rlvm_t
init_rlvm (uint64_t stack_size, uint64_t handler_size)
{
  return (rlvm_t)
  {
    .stack_size = stack_size,.handler_size = handler_size,.sp = 0,.ip =
      0,.esp = 0,.state = (status_t)
    {
    .state = CLEAN,.uid = 0}
    ,.iregs =
    {
    0}
    ,				/* Set to zero */
      .fregs =
    {
    0}
    ,				/* Set to zero */
  .stack =
      stack_size == 0 ? NULL : calloc (stack_size,
					   sizeof (uint64_t)),.estack =
      handler_size == 0 ? NULL : calloc (handler_size, sizeof (ehandle_t))};
}

void
print_rlvm_state (rlvm_t * vm)
{
  printf ("----- STATUS -----\n" "  SP: %zu\n" "  IP: %zu\n", vm->sp, vm->ip);
  size_t i;
  for (i = 0; i < ALLOC_REGS_COUNT; ++i)
    {
      printf ("  r%zu: %064" PRIx64 "\n", i, vm->iregs[i]);
    }
  for (i = 0; i < ALLOC_REGS_COUNT; ++i)
    {
      printf ("  fp%zu: %f\n", i, vm->fregs[i]);
    }
}

void
clean_rlvm (rlvm_t * vm)
{
  free (vm->stack);
  vm->stack_size = 0;
  vm->stack = NULL;
  free (vm->estack);
  vm->handler_size = 0;
  vm->estack = NULL;
}

static inline int64_t
__pad_sign_bit (uint64_t x, size_t width)
{
  const uint64_t max_val = 1 << --width;
  return (x & (max_val - 1)) - max_val * ((x >> (width)) & 1);
}

static inline uint64_t
__rotate_left (uint64_t x, size_t times)
{
  if (times % 64 == 0)
    return x;
  return x << times | x >> (64 - times);
}

static inline uint64_t
__rotate_right (uint64_t x, size_t times)
{
  if (times % 64 == 0)
    return x;
  return x >> times | x << (64 - times);
}

#define VM_THROW(vm, st, id, flbl)		\
  do						\
    {						\
      vm->state = (status_t) {			\
	.state = st,				\
	.uid = id				\
      };					\
      goto flbl;				\
    }						\
  while (0)

status_t
exec_bytecode (rlvm_t * vm, const uint64_t len, opcode_t * ops)
{
  while (vm->ip < len)
    {
      const opcode_t instr = ops[vm->ip];
      switch (instr.fvar.opcode)
	{
	  /* 6 bits means range is [0, 63] */
	case 0:
	  switch (instr.fvar.fn)
	    {
	    case 0:		/* op: HALT rs: r# */
	      VM_THROW (vm, CLEAN, vm->iregs[instr.fvar.rs], on_fault);
	    case 1:		/* op: MRI rs: r# rd: r# sa: acc */
	      switch (instr.fvar.sa)
		{
		case 0:	/* Plain-old copy */
		  vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs];
		  break;
		case 1:	/* Copy high 32 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rs] & 0xFFFFFFFF00000000) |
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFF);
		  break;
		case 2:	/* Copy low 32 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFF00000000) |
		    (vm->iregs[instr.fvar.rs] & 0xFFFFFFFF);
		  break;
		case 3:	/* Copy low 16 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFFFFFF0000) |
		    (vm->iregs[instr.fvar.rs] & 0xFFFF);
		  break;
		case 4:	/* Copy low 8 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFFFFFFFF00) |
		    (vm->iregs[instr.fvar.rs] & 0xFF);
		  break;
		}
	      break;
	    case 2:		/* op: MRF rs: r# rd: r# */
	      vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs];
	      break;
	    case 3:		/* op: SWPI rs: r# rd: r# */
	      vm->iregs[instr.fvar.rd] ^= vm->iregs[instr.fvar.rs];
	      vm->iregs[instr.fvar.rs] ^= vm->iregs[instr.fvar.rd];
	      vm->iregs[instr.fvar.rd] ^= vm->iregs[instr.fvar.rs];
	      break;
	    case 4:		/* op: ITF rs: r# rd: r# rt specifying mode */
	      switch (instr.fvar.rt)
		{
		case 0:	/* Float takes int's textual value */
		  vm->fregs[instr.fvar.rd] = instr.fvar.rs;
		  break;
		case 1:	/* Float takes int's bits */
		  {
		    union fp_i_conv_t conv = (union fp_i_conv_t) {
		      .ival = instr.fvar.rs
		    };
		    vm->fregs[instr.fvar.rd] = conv.fval;
		    break;
		  }
		}
	      break;
	    case 5:		/* op: FTI rs: r# rd: r# rt specifying mode */
	      switch (instr.fvar.rt)
		{
		case 0:	/* Int takes float's textual value, floor */
		  vm->iregs[instr.fvar.rd] = floor (vm->fregs[instr.fvar.rs]);
		  break;
		case 1:	/* Int takes float's bits */
		  {
		    union fp_i_conv_t conv = (union fp_i_conv_t) {
		      .fval = vm->fregs[instr.fvar.rs]
		    };
		    vm->iregs[instr.fvar.rd] = conv.ival;
		    break;
		  }
		case 2:	/* Int takes float's textual value, ceil */
		  vm->iregs[instr.fvar.rd] = ceil (vm->fregs[instr.fvar.rs]);
		  break;
		}
	      break;
	    case 6:		/* op: REH (remove exception handler) */
	      if (vm->esp == 0)
		VM_THROW (vm, STACK_UFLOW, 0, on_fault);
	      vm->esp -= 1;
	      continue;
	    case 7:		/* op: TRE rs: r# (throw exception) */
	      VM_THROW (vm, USER_DEFINED, vm->iregs[instr.fvar.rs], on_fault);
	    case 8:		/* op: STK rs: r# rt: r# rd: r# sa: n */
	      /* If sa has the thrid bit on, it means pop. Push otherwise */
	      if ((instr.fvar.sa & 4) == 1)
		{
		  if (vm->sp - (instr.fvar.sa & 2) == 0)
		    VM_THROW (vm, STACK_UFLOW, 0, on_fault);
		  switch (instr.fvar.sa & 2)
		    {
		    case 2:
		      vm->iregs[instr.fvar.rs] = vm->stack[--vm->sp];
		      vm->iregs[instr.fvar.rt] = vm->stack[--vm->sp];
		      vm->iregs[instr.fvar.rd] = vm->stack[--vm->sp];
		      break;
		    case 1:
		      vm->iregs[instr.fvar.rs] = vm->stack[--vm->sp];
		      vm->iregs[instr.fvar.rt] = vm->stack[--vm->sp];
		      break;
		    case 0:
		      vm->iregs[instr.fvar.rs] = vm->stack[--vm->sp];
		      break;
		    }
		  break;
		}
	      if (vm->sp >= vm->stack_size - (instr.fvar.sa & 2))
		VM_THROW (vm, STACK_OFLOW, 0, on_fault);
	      switch (instr.fvar.sa & 2)
		{
		case 2:
		  vm->stack[vm->sp++] = vm->iregs[instr.fvar.rd];
		  /* Intentional Fallthrough! */
		case 1:
		  vm->stack[vm->sp++] = vm->iregs[instr.fvar.rt];
		  /* Intentional Fallthrough! */
		case 0:
		  vm->stack[vm->sp++] = vm->iregs[instr.fvar.rs];
		  break;
		}
	      break;
	    case 9:		/* op: LDE rd: r# sa: subop */
	      switch (instr.fvar.sa)
		{
		case 2:	/* Push state onto the stack and load into rd */
		  vm->iregs[instr.fvar.rd] = vm->state.bytes;
		  /* Intentional Fallthrough! */
		case 1:	/* Push state onto the stack */
		  if (vm->sp >= vm->stack_size)
		    VM_THROW (vm, STACK_OFLOW, 0, on_fault);
		  vm->stack[vm->sp++] = vm->state.bytes;
		  break;
		case 0:	/* Load into rd */
		  vm->iregs[instr.fvar.rd] = vm->state.bytes;
		  break;
		}
	      break;
	    }
	  break;
	case 1:		/* op: @ALU rs: r# rt: r# rd: r# */
	  {
	    uint64_t rhs = vm->iregs[instr.fvar.rt];
	    switch (instr.fvar.fn >> 4)	/* top 2 bits specify the shifts */
	      {
	      case 0:
		break;
	      case 1:
		rhs <<= instr.fvar.sa;
		break;
	      case 2:
		rhs >>= instr.fvar.sa;
		break;
	      case 3:
		rhs = ((int64_t) rhs) >> instr.fvar.sa;
		break;
	      }
	    switch (instr.fvar.fn & 15)	/* remaining 4 bits specify the op */
	      {
		/* 4 bits means range is [0, 15] */
	      case 0:		/* rd = rs + rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] + rhs;
		break;
	      case 1:		/* rd = rs - rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] - rhs;
		break;
	      case 2:		/* rd = rs * rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] * rhs;
		break;
	      case 3:		/* rd = rs / rhs */
		if (rhs == 0)
		  VM_THROW (vm, DIV_BY_ZERO, 0, on_fault);
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] / rhs;
		break;
	      case 4:		/* rd = rs % rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] % rhs;
		break;
	      case 5:		/* rd = rs AND rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] & rhs;
		break;
	      case 6:		/* rd = rs OR rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] | rhs;
		break;
	      case 7:		/* rd = rs XOR rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] ^ rhs;
		break;
	      case 8:		/* rd = NOT rhs */
		vm->iregs[instr.fvar.rd] = ~rhs;
		break;
	      case 9:		/* rd = rs LSH rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] << rhs;
		break;
	      case 10:		/* rd = rs RSH rhs */
		vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs] >> rhs;
		break;
	      case 11:		/* rd = rs SRSH rhs */
		vm->iregs[instr.fvar.rd] =
		  ((int64_t) vm->iregs[instr.fvar.rs]) >> rhs;
		break;
	      case 12:		/* rd = rs ROL rhs */
		vm->iregs[instr.fvar.rd] =
		  __rotate_left (vm->iregs[instr.fvar.rs], rhs);
		break;
	      case 13:		/* rd = rs ROR rhs */
		vm->iregs[instr.fvar.rd] =
		  __rotate_right (vm->iregs[instr.fvar.rs], rhs);
		break;
	      }
	    break;
	  }
	case 2:
	  switch (instr.fvar.fn)
	    {
	    case 0:		/* rd = rs + rt [fp] */
	      vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] +
		vm->fregs[instr.fvar.rt];
	      break;
	    case 1:		/* rd = rs - rt [fp] */
	      vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] -
		vm->fregs[instr.fvar.rt];
	      break;
	    case 2:		/* rd = rs * rt [fp] */
	      vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] *
		vm->fregs[instr.fvar.rt];
	      break;
	    case 3:		/* rd = rs / rt [fp] */
	      vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] /
		vm->fregs[instr.fvar.rt];
	      break;
	    case 4:		/* rd = rs % rt [fp] */
	      vm->fregs[instr.fvar.rd] = fmod (vm->fregs[instr.fvar.rs],
					       vm->fregs[instr.fvar.rt]);
	      break;
	    }
	  break;
	case 3:		/* op: LDI rs: r# rt: << immediate: val */
	  vm->iregs[instr.svar.rs] = instr.svar.immediate << instr.svar.rt;
	  break;
	case 4:		/* op: ADDI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] + instr.svar.immediate;
	  break;
	case 5:		/* op: SUBI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] - instr.svar.immediate;
	  break;
	case 6:		/* op: MULI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] * instr.svar.immediate;
	  break;
	case 7:		/* op: DIVI rs: r# rt: r# immediate: val */
	  if (instr.svar.immediate == 0)
	    VM_THROW (vm, DIV_BY_ZERO, 0, on_fault);
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] / instr.svar.immediate;
	  break;
	case 8:		/* op: MODI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] % instr.svar.immediate;
	  break;
	case 9:		/* op: ANDI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] & instr.svar.immediate;
	  break;
	case 10:		/* op: ORI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] | instr.svar.immediate;
	  break;
	case 11:		/* op: XORI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] ^ instr.svar.immediate;
	  break;
	case 12:		/* op: CALL target: val */
	  if (vm->sp >= vm->stack_size)
	    VM_THROW (vm, STACK_OFLOW, 0, on_fault);
	  vm->stack[vm->sp++] = vm->ip + 1;	/* Intentional Fallthrough! */
	case 13:		/* op: JMP target: val */
	  vm->ip = instr.tvar.target;
	  continue;
	case 14:		/* op: RET */
	  if (vm->sp == 0)
	    VM_THROW (vm, STACK_UFLOW, 0, on_fault);
	  vm->ip = vm->stack[--vm->sp];
	  continue;
	case 15:		/* op: JE rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] == vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 16:		/* op: JL rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] < vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 17:		/* op: JG rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] > vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 18:		/* op: JSL rs: r# rt: r# immediate: val */
	  if ((int64_t) vm->iregs[instr.svar.rs] <
	      (int64_t) vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 19:		/* op: JSG rs: r# rt: r# immediate: val */
	  if ((int64_t) vm->iregs[instr.svar.rs] >
	      (int64_t) vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 20:		/* op: JFE rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] == vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 21:		/* op: JFL rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] < vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 22:		/* op: JFG rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] > vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 23:		/* op: JOF target: sval */
	  vm->ip += __pad_sign_bit (instr.tvar.target, 26);
	  continue;
	case 24:		/* op: JIR rs: r# rt: << immediate: sval */
	  vm->ip =
	    (vm->iregs[instr.svar.rs] << instr.svar.rt) +
	    __pad_sign_bit (instr.svar.immediate, 16);
	  continue;
	case 25:		/* op: JZ rs: r# rt: mode immediate: val */
	  if ((instr.svar.rt == 0) && (vm->iregs[instr.svar.rs] == 0))
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  if ((instr.svar.rt == 1) && (vm->fregs[instr.svar.rs] == 0))
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 26:		/* op: IEH target: val (set exception handler) */
	  if (vm->esp >= vm->handler_size)
	    VM_THROW (vm, STACK_OFLOW, 0, on_fault);
	  vm->estack[vm->esp++] = (ehandle_t)
	  {
	  .on_fault = instr.tvar.target,.old_sp = vm->sp};
	  break;
	case 27:		/* op: SLS rs: mode rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    if (vm->sp + offset >= vm->stack_size)
	      VM_THROW (vm, STACK_OFLOW, 0, on_fault);
	    switch (instr.fvar.rs)
	      {
	      case 0:		/* Load into iregs */
		vm->iregs[instr.svar.rt] = vm->stack[vm->sp + offset];
		break;
	      case 1:		/* Load into fregs (reinterpret cast) */
		{
		  union fp_i_conv_t conv = (union fp_i_conv_t) {
		    .ival = vm->stack[vm->sp + offset]
		  };
		  vm->fregs[instr.fvar.rt] = conv.fval;
		  break;
		}
	      case 2:		/* Store iregs onto stack */
		vm->stack[vm->sp + offset] = vm->iregs[instr.svar.rt];
		break;
	      case 3:		/* Store fregs onto stack (texture value) */
		vm->stack[vm->sp + offset] = floor (vm->fregs[instr.fvar.rt]);
		break;
	      case 4:		/* Store fregs onto stack (reinterpret cast) */
		{
		  union fp_i_conv_t conv = (union fp_i_conv_t) {
		    .fval = vm->fregs[instr.fvar.rt]
		  };
		  vm->stack[vm->sp + offset] = conv.ival;
		  break;
		}
	      }
	    break;
	  }
	case 28:		/* op: ALLOC rt: r# rs: r# immediate: val */
	  if (instr.svar.immediate == 0)
	    {
	      vm->iregs[instr.svar.rt] = (uint64_t) malloc (instr.svar.rs);
	    }
	  else
	    {
	      vm->iregs[instr.svar.rt] =
		(uint64_t) malloc (instr.svar.immediate);
	    }
	  break;
	case 29:		/* op: FREE rt: r# */
	  free ((void *) vm->iregs[instr.svar.rt]);
	  break;
	case 30:		/* op: HLDB rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint8_t *ptr = (uint8_t *) (((char *) instr.fvar.rs) + offset);
	    vm->iregs[instr.svar.rt] = *ptr;
	    break;
	  }
	case 31:		/* op: HLDW rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint16_t *ptr = (uint16_t *) (((char *) instr.fvar.rs) + offset);
	    vm->iregs[instr.svar.rt] = *ptr;
	    break;
	  }
	case 32:		/* op: HLDD rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint32_t *ptr = (uint32_t *) (((char *) instr.fvar.rs) + offset);
	    vm->iregs[instr.svar.rt] = *ptr;
	    break;
	  }
	case 33:		/* op: HLDQ rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint64_t *ptr = (uint64_t *) (((char *) instr.fvar.rs) + offset);
	    vm->iregs[instr.svar.rt] = *ptr;
	    break;
	  }
	case 34:		/* op: HSTB rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint8_t *ptr = (uint8_t *) (((char *) instr.fvar.rs) + offset);
	    *ptr = vm->iregs[instr.svar.rt];
	    break;
	  }
	case 35:		/* op: HSTW rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint16_t *ptr = (uint16_t *) (((char *) instr.fvar.rs) + offset);
	    *ptr = vm->iregs[instr.svar.rt];
	    break;
	  }
	case 36:		/* op: HSTD rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint32_t *ptr = (uint32_t *) (((char *) instr.fvar.rs) + offset);
	    *ptr = vm->iregs[instr.svar.rt];
	    break;
	  }
	case 37:		/* op: HSTQ rs: base rt: r# immediate: signed offset */
	  {
	    const int64_t offset = __pad_sign_bit (instr.svar.immediate, 16);
	    uint64_t *ptr = (uint64_t *) (((char *) instr.fvar.rs) + offset);
	    *ptr = vm->iregs[instr.svar.rt];
	    break;
	  }
	case 38:		/* op: SCJMP rs: r# rt: r# immediate: flag */
	  {
	    /*
	     * Float point related will have the 4th bit on.
	     * < and > have 3rd bit for unsigned int comparisons
	     */
	    bool rst = false;
	    switch (instr.svar.immediate & 7)
	      {
	      case 0:		/* equal */
		rst =
		  (instr.svar.immediate & 8) ? (vm->fregs[instr.svar.rs] ==
						vm->fregs[instr.
							  svar.rt])
		  : (vm->iregs[instr.svar.rs] == vm->iregs[instr.svar.rt]);
		break;
	      case 1:		/* lesser than */
		if (instr.svar.immediate & 8)
		  rst = vm->fregs[instr.svar.rs] < vm->fregs[instr.svar.rt];
		else if (instr.svar.immediate & 4)
		  rst =
		    ((int64_t) vm->iregs[instr.svar.rs]) <
		    ((int64_t) vm->iregs[instr.svar.rt]);
		else
		  rst = vm->iregs[instr.svar.rs] < vm->iregs[instr.svar.rt];
		break;
	      case 2:		/* greater than */
		if (instr.svar.immediate & 8)
		  rst = vm->fregs[instr.svar.rs] > vm->fregs[instr.svar.rt];
		else if (instr.svar.immediate & 4)
		  rst =
		    ((int64_t) vm->iregs[instr.svar.rs]) >
		    ((int64_t) vm->iregs[instr.svar.rt]);
		else
		  rst = vm->iregs[instr.svar.rs] > vm->iregs[instr.svar.rt];
		break;
	      case 3:		/* zero */
		rst =
		  (instr.svar.immediate & 8) ? (vm->fregs[instr.svar.rs] ==
						0) : (vm->iregs[instr.
								svar.rs] ==
						      0);
		break;
	      }
	    /*
	     * If rst is true, the next line is executed.
	     * The next line is skipped otherwise.
	     */
	    if (!rst)
	      vm->ip += 1;
	    break;
	  }
	default:
	  VM_THROW (vm, BAD_OPCODE, instr.bytes, on_fault);
	}
      vm->ip += 1;
      continue;
    on_fault:
      /* If state is set to CLEAN, that means it was a halt instruction */
      if (vm->state.state == CLEAN)
	break;
      /*
       * Check which handler should be used. Due to the way the handlers
       * are done, jumping into a try block is not a good idea. If that
       * happens, the try block will not be registered and you will end
       * up triggering another catch block.
       */
      if (vm->esp == 0)
	break;
      const ehandle_t handle = vm->estack[--vm->esp];
      vm->ip = handle.on_fault;
      while (vm->sp != handle.old_sp)
	vm->sp -= 1;
    }
  return vm->state;
}
