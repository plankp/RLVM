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

#include "instrbuf.h"

instrbuf_t
init_buf (size_t cap)
{
  return (instrbuf_t)
  {
  .size = 0,.cap = cap,.ptr = calloc (sizeof (opcode_t), cap)};
}

void
ensure_cap (instrbuf_t * buf, size_t cap)
{
  size_t pcap = buf->cap;
  if (pcap >= cap)
    return;
  while (pcap < cap)
    pcap *= 2;
  opcode_t *nptr = realloc (buf->ptr, pcap * sizeof (opcode_t));
  if (nptr != NULL)
    {
      buf->cap = pcap;
      buf->ptr = nptr;
    }
}

void
push (instrbuf_t * buf, opcode_t instr)
{
  ensure_cap (buf, buf->size + 1);
  buf->ptr[buf->size++] = instr;
}

void
pop (instrbuf_t * buf)
{
  if (buf->size == 0)
    return;
  --buf->size;
}

void
free_buf (instrbuf_t * buf)
{
  free (buf->ptr);
  buf->ptr = NULL;
}
