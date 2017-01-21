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

#include "bcode.h"

bcode_t *
read_bytecode (FILE * f, bcode_t * bf)
{
  if (bf == NULL)
    return NULL;
  /*
   * Assuming parameter f is opened as a binary file!
   *
   * Each bytecode file begins with a magic number
   * composed of two chars ('\x2C', '\xDF').
   */
  if (fread (&bf->magic, sizeof (bf->magic), 1, f) != 1)
    return NULL;
  if (bf->magic[0] != 0x2C)
    return NULL;
  if (!(bf->magic[1] == 0xDF || bf->magic[1] == 0xD0))
    return NULL;

  if (fread (&bf->cstack_size, sizeof (bf->cstack_size), 1, f) != 1)
    return NULL;
  if (bf->magic[1] == 0xDF)
    bf->cstack_size = be64toh (bf->cstack_size);
  else
    bf->cstack_size = le64toh (bf->cstack_size);

  if (fread (&bf->estack_size, sizeof (bf->estack_size), 1, f) != 1)
    return NULL;
  if (bf->magic[1] == 0xDF)
    bf->cstack_size = be64toh (bf->estack_size);
  else
    bf->estack_size = le64toh (bf->estack_size);

  if (fread (&bf->code_size, sizeof (bf->code_size), 1, f) != 1)
    return NULL;
  if (bf->magic[1] == 0xDF)
    bf->code_size = be64toh (bf->code_size);
  else
    bf->code_size = le64toh (bf->code_size);

  bf->code = calloc (bf->code_size, sizeof (opcode_t));
  if (fread (bf->code, sizeof (opcode_t), bf->code_size, f) != bf->code_size)
    return NULL;
  size_t i;
  if (bf->magic[1] == 0xDF)
    {
      for (i = 0; i < bf->code_size; ++i)
	{
	  bf->code[i].bytes = be64toh (bf->code[i].bytes);
	}
    }
  else
    {
      for (i = 0; i < bf->code_size; ++i)
	{
	  bf->code[i].bytes = le64toh (bf->code[i].bytes);
	}
    }
  return bf;
}

bool
write_bytecode (FILE * f, bcode_t * bf)
{
  if (bf == NULL)
    return false;
  /*
   * Assuming parameter f is opened as binary file
   *
   * Validate parameter bf before doing any writing
   * (the magic header).
   */
  if (bf->magic[0] != 0x2C)
    return false;
  if (!(bf->magic[1] == 0xDF || bf->magic[1] == 0xD0))
    return false;

  if (fwrite (bf->magic, sizeof (uint8_t), 2, f) != 2)
    return false;
  if (fwrite (&bf->cstack_size, sizeof (uint64_t), 1, f) != 1)
    return false;
  if (fwrite (&bf->estack_size, sizeof (uint64_t), 1, f) != 1)
    return false;
  if (fwrite (&bf->code_size, sizeof (uint64_t), 1, f) != 1)
    return false;
  if (fwrite (bf->code, sizeof (opcode_t), bf->code_size, f) != bf->code_size)
    return false;
  return true;
}

status_t
exec_bcode_t (rlvm_t * vm, bcode_t * bf)
{
  rlvm_t lvm = init_rlvm (bf->cstack_size, bf->estack_size);
  memcpy (vm, &lvm, sizeof (rlvm_t));
  return exec_bytecode (vm, bf->code_size, bf->code);
}

void
clean_bcode (bcode_t * bf)
{
  free (bf->code);
  bf->code = NULL;
}
