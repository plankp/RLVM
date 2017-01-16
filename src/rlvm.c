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
init_rlvm (size_t stack_size)
{
  return (rlvm_t)
  {
    .stack_size = stack_size,.sp = 0,.ip = 0,.iregs =
    {
    0}
    ,				/* Set to zero */
      .fregs =
    {
    0}
    ,				/* Set to zero */
  .stack = stack_size == 0 ? NULL : calloc (stack_size, sizeof (size_t))};
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
}

static inline int64_t
__pad_sign_bit (uint64_t x, size_t width)
{
  const uint64_t max_val = 1 << --width;
  return ((x & (max_val - 1)) - max_val * ((x >> (width)) & 1));
}

int
exec_bytecode (rlvm_t * vm, const size_t len, opcode_t * ops)
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
	      return (int) vm->iregs[instr.fvar.rs];
	    case 1:		/* op: MRI rs: r# rd: r# sa: acc */
	      switch (instr.fvar.sa)
		{
		case 0:	/* Plain-old copy */
		  vm->iregs[instr.fvar.rd] = vm->iregs[instr.fvar.rs];
		  break;
		case 1:	/* Copy high 32 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rs] & 0xFFFFFFFF00000000) | (vm->
								       iregs
								       [instr.
									fvar.
									rd] &
								       0xFFFFFFFF);
		  break;
		case 2:	/* Copy low 32 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFF00000000) | (vm->
								       iregs
								       [instr.
									fvar.
									rs] &
								       0xFFFFFFFF);
		  break;
		case 3:	/* Copy low 16 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFFFFFF0000) | (vm->
								       iregs
								       [instr.
									fvar.
									rs] &
								       0xFFFF);
		  break;
		case 4:	/* Copy low 8 bits */
		  vm->iregs[instr.fvar.rd] =
		    (vm->iregs[instr.fvar.rd] & 0xFFFFFFFFFFFFFF00) | (vm->
								       iregs
								       [instr.
									fvar.
									rs] &
								       0xFF);
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
		rhs = ((int16_t) rhs) >> instr.fvar.sa;
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
		/* End of integer oriented instructions */
	      case 9:		/* rd = rs + rt [fp] */
		vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] +
		  vm->fregs[instr.fvar.rt];
		break;
	      case 10:		/* rd = rs - rt [fp] */
		vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] -
		  vm->fregs[instr.fvar.rt];
		break;
	      case 11:		/* rd = rs * rt [fp] */
		vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] *
		  vm->fregs[instr.fvar.rt];
		break;
	      case 12:		/* rd = rs / rt [fp] */
		vm->fregs[instr.fvar.rd] = vm->fregs[instr.fvar.rs] /
		  vm->fregs[instr.fvar.rt];
		break;
	      case 13:		/* rd = rs % rt [fp] */
		vm->fregs[instr.fvar.rd] = fmod (vm->fregs[instr.fvar.rs],
						 vm->fregs[instr.fvar.rt]);
		break;
	      case 14:		/* fp::rd = i::rhs with rs specifying mode */
		switch (instr.fvar.rs)
		  {
		  case 0:	/* Float takes int's textual value */
		    vm->fregs[instr.fvar.rd] = rhs;
		    break;
		  case 1:	/* Float takes int's bits */
		    {
		      union fp_i_conv_t conv = (union fp_i_conv_t) {
			.ival = rhs
		      };
		      vm->fregs[instr.fvar.rd] = conv.fval;
		      break;
		    }
		  }
		break;
	      case 15:		/* i::rd = fp::rs with rt specifying mode */
		switch (instr.fvar.rt)
		  {
		  case 0:	/* Int takes float's textual value, floor */
		    vm->iregs[instr.fvar.rd] =
		      floor (vm->fregs[instr.fvar.rs]);
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
		    vm->iregs[instr.fvar.rd] =
		      ceil (vm->fregs[instr.fvar.rs]);
		    break;
		  }
		break;
		/* End of float point oriented instructions */
	      }
	    break;
	  }
	case 2:		/* op: LDI rs: r# rt: << immediate: val */
	  vm->iregs[instr.svar.rs] = instr.svar.immediate << instr.svar.rt;
	  break;
	case 3:		/* op: ADDI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] + instr.svar.immediate;
	  break;
	case 4:		/* op: SUBI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] - instr.svar.immediate;
	  break;
	case 5:		/* op: MULI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] * instr.svar.immediate;
	  break;
	case 6:		/* op: DIVI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] / instr.svar.immediate;
	  break;
	case 7:		/* op: MODI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] % instr.svar.immediate;
	  break;
	case 8:		/* op: ANDI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] & instr.svar.immediate;
	  break;
	case 9:		/* op: ORI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] | instr.svar.immediate;
	  break;
	case 10:		/* op: XORI rs: r# rt: r# immediate: val */
	  vm->iregs[instr.svar.rs] =
	    vm->iregs[instr.svar.rt] ^ instr.svar.immediate;
	  break;
	case 11:		/* op: CALL target: val */
	  if (vm->sp >= vm->stack_size)
	    return 1;		/* ERR:STACK OVERFLOW */
	  vm->stack[vm->sp++] = vm->ip + 1;	/* Intentional Fallthrough! */
	case 12:		/* op: JMP target: val */
	  vm->ip = instr.tvar.target;
	  continue;
	case 13:
	  if (vm->sp == 0)
	    return 1;		/* ERR: STACK UNDERFLOW */
	  vm->ip = vm->stack[--vm->sp];
	  continue;
	case 15:		/* op: JE rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] == vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 17:		/* op: JL rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] < vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 18:		/* op: JG rs: r# rt: r# immediate: val */
	  if (vm->iregs[instr.svar.rs] > vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 19:		/* op: JSL rs: r# rt: r# immediate: val */
	  if ((int64_t) vm->iregs[instr.svar.rs] <
	      (int64_t) vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 20:		/* op: JSG rs: r# rt: r# immediate: val */
	  if ((int64_t) vm->iregs[instr.svar.rs] >
	      (int64_t) vm->iregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 21:		/* op: JFE rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] == vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 22:		/* op: JFL rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] < vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 23:		/* op: JFG rs: r# rt: r# immediate: val */
	  if (vm->fregs[instr.svar.rs] > vm->fregs[instr.svar.rt])
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	case 24:		/* op: JOF target: sval */
	  vm->ip += __pad_sign_bit (instr.tvar.target, 26);
	  continue;
	case 25:		/* op: JIR rs: r# rt: << immediate: sval */
	  vm->ip =
	    (vm->iregs[instr.svar.rs] << instr.svar.rt) +
	    instr.svar.immediate;
	  continue;
	case 26:		/* op: JZ rs: r# rt: (not used) immediate: val */
	  if (vm->iregs[instr.svar.rs] == 0)
	    {
	      vm->ip = instr.svar.immediate;
	      continue;
	    }
	  break;
	}
      vm->ip += 1;
    }
  return 0;
}
