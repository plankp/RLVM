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

#include <endian.h>
#include "rlvm.h"

void
print_binary (int num, size_t width)
{
  while (width > 0)
    {
      width -= 1;
      putc ((num & 1) + '0', stdout);
      num >>= 1;
    }
}

int
main (void)
{
  rlvm_t vm = init_rlvm (0);

  /*
   * Calculate the gcd of 48 and 180
   * 0   ldi r0, 48
   * 1   ldi r1, 180
   * 2   jz r1, 6
   * 3   mod r0, r0, r1
   * 4   swpi r0, r1
   * 5   jof -3
   * 6   hlt r0              # r0 already contains the result
   */
  opcode_t ins[] = {
    {
     .svar = (op_svar_t) {
			  .opcode = 3,.rs = 0,.rt = 0,.immediate = 48}
     },
    {
     .svar = (op_svar_t) {
			  .opcode = 3,.rs = 1,.rt = 0,.immediate = 180}
     },
    {
     .svar = (op_svar_t) {
			  .opcode = 25,.rs = 1,.rt = 0,.immediate = 6}
     },
    {
     .fvar = (op_fvar_t) {
			  .opcode = 1,.rs = 0,.rt = 1,.rd = 0,.sa = 0,.fn = 4}
     },
    {
     .fvar = (op_fvar_t) {
			  .opcode = 0,.rs = 0,.rd = 1,.rt = 0,.sa = 0,.fn = 3}
     },
    {
     .tvar = (op_tvar_t) {
			  .opcode = 23,.target = -3}
     },
    {
     .bytes = 0}
  };

  /* This only works because ins is a *legit* array (as opposed to a pointer) */
  const size_t length = sizeof (ins) / sizeof (ins[0]);
  printf ("Executing bytes:\n");
  size_t i;
  for (i = 0; i < length; ++i)
    {
      print_binary (ins[i].bytes, 32);
      printf ("\n");
    }

  status_t retval = exec_bytecode (&vm, length, ins);
  if (retval.state != CLEAN)
    printf ("VM EXITED WITH NON-ZERO VALUE! (%d: %d)\n", retval.state,
	    retval.uid);

  print_rlvm_state (&vm);
  clean_rlvm (&vm);
  return 0;
}
