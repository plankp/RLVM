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

static void
print_fvar_opcode (op_fvar_t op, FILE * out)
{
  fprintf (out,
	   "%06x %05x %05x %05x %05x %06x",
	   op.opcode, op.rs, op.rt, op.rd, op.sa, op.fn);
}

static void
print_svar_opcode (op_svar_t op, FILE * out)
{
  fprintf (out,
	   "%06x %05x %05x %016x  ", op.opcode, op.rs, op.rt, op.immediate);
}

static void
print_tvar_opcode (op_tvar_t op, FILE * out)
{
  fprintf (out, "%06x %026x    ", op.opcode, op.target);
}

typedef void (*disf_t) (opcode_t, FILE *);

static void
dis_opcode_0 (opcode_t opcode, FILE * out)
{
  print_fvar_opcode (opcode.fvar, out);
  switch (opcode.fvar.fn)
    {
    case 0:
      fprintf (out, " halt r%d\n", opcode.fvar.rs);
      break;
    case 1:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, " mov r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, " mh32 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 2:
	  fprintf (out, " ml32 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 3:
	  fprintf (out, " ml16 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 4:
	  fprintf (out, " ml8 r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 2:
      fprintf (out, " mov fp%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 3:
      fprintf (out, " swp r%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
      break;
    case 4:
      switch (opcode.fvar.rt)
	{
	case 0:
	  fprintf (out, " i2f fp%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, " b2f fp%d,r%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 5:
      switch (opcode.fvar.rt)
	{
	case 0:
	  fprintf (out, " f2if r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, " f2b r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	case 2:
	  fprintf (out, " f2ic r%d,fp%d\n", opcode.fvar.rd, opcode.fvar.rs);
	  break;
	}
      break;
    case 6:
      fprintf (out, " rmeh\n");
      break;
    case 7:
      fprintf (out, " throw r%d\n", opcode.fvar.rs);
      break;
    case 8:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, " push r%d\n", opcode.fvar.rs);
	  break;
	case 1:
	  fprintf (out, " push r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt);
	  break;
	case 2:
	  fprintf (out, " push r%d,r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt,
		   opcode.fvar.rd);
	  break;
	case 4:
	  fprintf (out, " pop r%d\n", opcode.fvar.rs);
	  break;
	case 5:
	  fprintf (out, " pop r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt);
	  break;
	case 6:
	  fprintf (out, " pop r%d,r%d,r%d\n", opcode.fvar.rs, opcode.fvar.rt,
		   opcode.fvar.rd);
	  break;
	}
      break;
    case 9:
      switch (opcode.fvar.sa)
	{
	case 0:
	  fprintf (out, " ldex r%d\n", opcode.fvar.rd);
	  break;
	case 1:
	  fprintf (out, " ldex\n");
	  break;
	case 2:
	  fprintf (out, " pldex r%d\n", opcode.fvar.rd);
	  break;
	}
    }
}

static void
dis_opcode_1 (opcode_t opcode, FILE * out)
{
  print_fvar_opcode (opcode.fvar, out);
  switch (opcode.fvar.fn & 15)
    {
    case 0:
      printf (" add r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 1:
      printf (" sub r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 2:
      printf (" mul r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 3:
      printf (" div r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 4:
      printf (" mod r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 5:
      printf (" and r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 6:
      printf (" or r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 7:
      printf (" xor r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 8:
      printf (" not r%d,r%d", opcode.fvar.rd, opcode.fvar.rt);
      break;
    case 9:
      printf (" lsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 10:
      printf (" rsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 11:
      printf (" srsh r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 12:
      printf (" rol r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    case 13:
      printf (" ror r%d,r%d,r%d", opcode.fvar.rd, opcode.fvar.rs,
	      opcode.fvar.rt);
      break;
    }
  switch (opcode.fvar.fn >> 4)
    {
    case 0:
      printf ("\n");
      break;
    case 1:
      printf (",LSH %u\n", opcode.fvar.sa);
      break;
    case 2:
      printf (",RSH %u\n", opcode.fvar.sa);
      break;
    case 3:
      printf (",SRSH %u\n", opcode.fvar.sa);
      break;
    }
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

      static const disf_t dis_table[] = { &dis_opcode_0, &dis_opcode_1 };
      static const size_t dtab_len =
	sizeof (dis_table) / sizeof (dis_table[0]);

      uint64_t ip;
      for (ip = 0; ip < code[i].code_size; ++ip)
	{
	  fprintf (out, "%016" PRIx64 "  ", ip);
	  const opcode_t instr = code[i].code[ip];
	  if (instr.fvar.opcode < dtab_len)
	    dis_table[instr.fvar.opcode] (instr, out);
	  else
	    fprintf (out, "(Unsupported instruction)\n");
	}
    }
  return 0;
}
