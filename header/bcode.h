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
