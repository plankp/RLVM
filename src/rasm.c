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

#include "rasm.h"

typedef void (*disf_t) (opcode_t, FILE *);

static void
dis_opcode_0 (opcode_t opcode, FILE * out)
{
  switch (opcode.fvar.fn)
    {
    case 0:
      fprintf (out, "halt r%d\n", opcode.fvar.rs);
      break;
    case 1:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, "mov r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, "mh32 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 2:
	  fprintf (out, "ml32 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 3:
	  fprintf (out, "ml16 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 4:
	  fprintf (out, "ml8 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 2:
      fprintf (out, "mov fp%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 3:
      fprintf (out, "swp r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 4:
      switch (opcode.fvar.rt)
	{
	case 0:
	  fprintf (out, "i2f fp%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, "b2f fp%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 5:
      switch (opcode.fvar.rt)
	{
	case 0:
	  fprintf (out, "f2if r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, "f2b r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 2:
	  fprintf (out, "f2ic r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 6:
      fprintf (out, "rmeh\n");
      break;
    case 7:
      fprintf (out, "throw r%d\n", opcode.fvar.rs);
      break;
    case 8:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, "push r%d\n", opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, "push r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt);
	  break;
	case 2:
	  fprintf (out, "push r%d,r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt,
		   opcode.fvar.rd);
	  break;
	case 4:
	  fprintf (out, "pop r%d\n", opcode.fvar.rs);
	  break;
	case 5:
	  fprintf (out, "pop r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt);
	  break;
	case 6:
	  fprintf (out, "pop r%d,r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt,
		   opcode.fvar.rd);
	  break;
	}
      break;
    case 9:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, "ldex r%d\n", opcode.fvar.rd);
	  break;
	case 1:
	  fprintf (out, "ldex\n");
	  break;
	case 2:
	  fprintf (out, "pldex r%d\n", opcode.fvar.rd);
	  break;
	}
    }
}

static void
dis_opcode_1 (opcode_t opcode, FILE * out)
{
  switch (opcode.fvar.fn & 15)
    {
    case 0:
      fprintf (out, "add r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 1:
      fprintf (out, "sub r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 2:
      fprintf (out, "mul r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 3:
      fprintf (out, "div r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 4:
      fprintf (out, "mod r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 5:
      fprintf (out, "and r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 6:
      fprintf (out, "or r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 7:
      fprintf (out, "xor r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 8:
      fprintf (out, "not r%d,r%d", opcode.fvar.rd, opcode.fvar.rt);
      break;
    case 9:
      fprintf (out, "lsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 10:
      fprintf (out, "rsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 11:
      fprintf (out, "srsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 12:
      fprintf (out, "rol r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 13:
      fprintf (out, "ror r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    }
  switch (opcode.fvar.fn >> 4)
    {
    case 0:
      fprintf (out, "\n");
      break;
    case 1:
      fprintf (out, ",LSH %u\n", opcode.fvar.sa);
      break;
    case 2:
      fprintf (out, ",RSH %u\n", opcode.fvar.sa);
      break;
    case 3:
      fprintf (out, ",SRSH %u\n", opcode.fvar.sa);
      break;
    }
}

static void
dis_opcode_2 (opcode_t opcode, FILE * out)
{
  switch (opcode.fvar.fn)
    {
    case 0:
      fprintf (out, "add fp%d,fp%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 1:
      fprintf (out, "sub fp%d,fp%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 2:
      fprintf (out, "mul fp%d,fp%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 3:
      fprintf (out, "div fp%d,fp%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 4:
      fprintf (out, "mod fp%d,fp%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    }
}

static void
dis_opcode_3 (opcode_t opcode, FILE * out)
{
  fprintf (out, "mov r%d,%u", opcode.svar.rs, opcode.svar.immediate);
  if (opcode.svar.rt > 0)
    fprintf (out, ",LSH %u", opcode.svar.rt);
  fprintf (out, "\n");
}

static inline void
__dis_alu_immediate (opcode_t opcode, FILE * out, char *op)
{
  fprintf (out, "%s r%d,r%d,%u\n", op, opcode.svar.rs, opcode.svar.rt,
	   opcode.svar.immediate);
}

static inline void
__dis_flt_immediate (opcode_t opcode, FILE * out, char *op)
{
  fprintf (out, "%s fp%d,fp%d,%u\n", op, opcode.svar.rs, opcode.svar.rt,
	   opcode.svar.immediate);
}

static void
dis_opcode_4 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "add");
}

static void
dis_opcode_5 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "sub");
}

static void
dis_opcode_6 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "mul");
}

static void
dis_opcode_7 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "div");
}

static void
dis_opcode_8 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "mod");
}

static void
dis_opcode_9 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "and");
}

static void
dis_opcode_10 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "or");
}

static void
dis_opcode_11 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "xor");
}

static void
dis_opcode_12 (opcode_t opcode, FILE * out)
{
  fprintf (out, "call %u\n", opcode.tvar.target);
}

static void
dis_opcode_13 (opcode_t opcode, FILE * out)
{
  fprintf (out, "jmp %u\n", opcode.tvar.target);
}

static void
dis_opcode_14 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ret %u\n", opcode.tvar.target);
}

static void
dis_opcode_15 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "je");
}

static void
dis_opcode_16 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "jl");
}

static void
dis_opcode_17 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "jg");
}

static void
dis_opcode_18 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "jls");
}

static void
dis_opcode_19 (opcode_t opcode, FILE * out)
{
  __dis_alu_immediate (opcode, out, "jgs");
}

static void
dis_opcode_20 (opcode_t opcode, FILE * out)
{
  __dis_flt_immediate (opcode, out, "je");
}

static void
dis_opcode_21 (opcode_t opcode, FILE * out)
{
  __dis_flt_immediate (opcode, out, "jl");
}

static void
dis_opcode_22 (opcode_t opcode, FILE * out)
{
  __dis_flt_immediate (opcode, out, "jg");
}

static void
dis_opcode_23 (opcode_t opcode, FILE * out)
{
  fprintf (out, "jof %u\n", opcode.tvar.target);
}

static void
dis_opcode_24 (opcode_t opcode, FILE * out)
{
  fprintf (out, "jmp r%d", opcode.svar.rs);
  if (opcode.svar.rt > 0)
    fprintf (out, ",LSH %u", opcode.svar.rt);
  if (opcode.svar.immediate > 0)
    fprintf (out, ",%u", opcode.svar.immediate);
  fprintf (out, "\n");
}

static void
dis_opcode_25 (opcode_t opcode, FILE * out)
{
  switch (opcode.svar.rt)
    {
    case 0:
      fprintf (out, "jz r%d\n", opcode.svar.rs);
      break;
    case 1:
      fprintf (out, "jz fp%d\n", opcode.svar.rs);
      break;
    }
}

static void
dis_opcode_26 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ineh %u\n", opcode.tvar.target);
}

static void
dis_opcode_27 (opcode_t opcode, FILE * out)
{
  switch (opcode.svar.rs)
    {
    case 0:
      fprintf (out, "ld r%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
      break;
    case 1:
      fprintf (out, "ld fp%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
      break;
    case 2:
      fprintf (out, "st r%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
      break;
    case 3:
      fprintf (out, "st fp%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
      break;
    case 4:
      fprintf (out, "stfb fp%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
      break;
    }
}

static void
dis_opcode_28 (opcode_t opcode, FILE * out)
{
  if (opcode.svar.immediate == 0)
    fprintf (out, "alloc r%d,r%d\n", opcode.svar.rt, opcode.svar.rs);
  else
    fprintf (out, "alloc r%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
}

static void
dis_opcode_29 (opcode_t opcode, FILE * out)
{
  fprintf (out, "free r%d\n", opcode.svar.rt);
}

static void
dis_opcode_30 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldb r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_31 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldw r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_32 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldd r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_33 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldq r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_34 (opcode_t opcode, FILE * out)
{
  fprintf (out, "stb r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_35 (opcode_t opcode, FILE * out)
{
  fprintf (out, "stw r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_36 (opcode_t opcode, FILE * out)
{
  fprintf (out, "std r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_37 (opcode_t opcode, FILE * out)
{
  fprintf (out, "stq r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_38 (opcode_t opcode, FILE * out)
{
  fprintf (out, "sj");
  switch (opcode.svar.immediate & 3)
    {
    case 0:
      fprintf (out, "e");
      break;
    case 1:
      if (opcode.svar.immediate & 4)
	fprintf (out, "s");
      fprintf (out, "l");
      break;
    case 2:
      if (opcode.svar.immediate & 4)
	fprintf (out, "s");
      fprintf (out, "g");
      break;
    case 3:
      fprintf (out, "z ");
      fprintf (out, (opcode.svar.immediate & 8) ? "fp%d\n" : "r%d\n",
	       opcode.svar.rs);
      return;
    }
  fprintf (out, " ");
  fprintf (out, (opcode.svar.immediate & 8) ? "fp%d,fp%d\n" : "r%d,r%d\n",
	   opcode.svar.rs, opcode.svar.rt);
}

static void
dis_opcode_39 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldc r%d,r%d,%u\n", opcode.svar.rt, opcode.svar.rs,
	   opcode.svar.immediate);
}

static void
dis_opcode_40 (opcode_t opcode, FILE * out)
{
  fprintf (out, "ldc r%d,%u\n", opcode.svar.rt, opcode.svar.immediate);
}

static void
dis_opcode_41 (opcode_t opcode, FILE * out)
{
  switch (opcode.fvar.fn)
    {
    case 0:
      fprintf (out, "fread r%d,r%d", opcode.fvar.rs, opcode.fvar.rd);
      break;
    case 1:
      fprintf (out, "fread r%d,r%d,r%d", opcode.fvar.rs, opcode.fvar.rd,
	       opcode.fvar.rt);
      break;
    case 2:
      fprintf (out, "fread fp%d,r%d,r%d", opcode.fvar.rs, opcode.fvar.rd,
	       opcode.fvar.rt);
      break;
    case 3:
      fprintf (out, "fwrtb r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 4:
      fprintf (out, "fwrtq r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 5:
      fprintf (out, "fwrtq r%d,r%d,fp%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 6:
      fprintf (out, "fwrts r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 7:
      fprintf (out, "ldc r%d,<stdout>", opcode.fvar.rd);
      break;
    case 8:
      fprintf (out, "ldc r%d,<stderr>", opcode.fvar.rd);
      break;
    case 9:
      fprintf (out, "ldc r%d,<stdin>", opcode.fvar.rd);
      break;
    case 10:
      fprintf (out, "fopen r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	       opcode.fvar.rt);
      break;
    case 11:
      fprintf (out, "fclose r%d,r%d", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 12:
      fprintf (out, "fflush r%d,r%d", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 13:
      fprintf (out, "frewind r%d", opcode.fvar.rs);
      break;
    }
  fprintf (out, "\n");
}

int
disassemble (bcode_t * code, size_t count, FILE * out)
{
  size_t i;
  for (i = 0; i < count; ++i)
    {
      /* Ignore endianess because it should have already been dealt with */
      if (!(code[i].magic[0] == 0x2C &&
	    (code[i].magic[1] == 0xDF || code[i].magic[1] == 0xD0)))
	{
	  fprintf (out, "error: invalid magic header of 0x%x 0x%x\n",
		   code[i].magic[0], code[i].magic[1]);
	  return 1;
	}
      fprintf (out,
	       "Code stack size:    %" PRIu64 "\n"
	       "Error stack size:   %" PRIu64 "\n"
	       "Data pool size:     %" PRIu64 "\n\n"
	       "Disassembly of section TEXT\n",
	       code[i].cstack_size, code[i].estack_size, code[i].ropool_size);

      static const disf_t dis_table[] =
	{ &dis_opcode_0, &dis_opcode_1, &dis_opcode_2, &dis_opcode_3,
	&dis_opcode_4, &dis_opcode_5, &dis_opcode_6, &dis_opcode_7,
	&dis_opcode_8, &dis_opcode_9, &dis_opcode_10, &dis_opcode_11,
	&dis_opcode_12, &dis_opcode_13, &dis_opcode_14, &dis_opcode_15,
	&dis_opcode_16, &dis_opcode_17, &dis_opcode_18, &dis_opcode_19,
	&dis_opcode_20, &dis_opcode_21, &dis_opcode_22, &dis_opcode_23,
	&dis_opcode_24, &dis_opcode_25, &dis_opcode_26, &dis_opcode_27,
	&dis_opcode_28, &dis_opcode_29, &dis_opcode_30, &dis_opcode_31,
	&dis_opcode_32, &dis_opcode_33, &dis_opcode_34, &dis_opcode_35,
	&dis_opcode_36, &dis_opcode_37, &dis_opcode_38, &dis_opcode_39,
	&dis_opcode_40, &dis_opcode_41
      };
      static const size_t dtab_len =
	sizeof (dis_table) / sizeof (dis_table[0]);

      uint64_t ip;
      for (ip = 0; ip < code[i].code_size; ++ip)
	{
	  fprintf (out, "%016" PRIx64 ":   ", ip);
	  const opcode_t instr = code[i].code[ip];
	  fprintf (out, "%08" PRIx32 "    ", instr.bytes);
	  if (instr.fvar.opcode < dtab_len)
	    dis_table[instr.fvar.opcode] (instr, out);
	  else
	    fprintf (out, "(Unsupported instruction)\n");
	}
    }
  return 0;
}
