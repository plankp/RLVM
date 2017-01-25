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

#ifndef __LBLMAP_H__
#define __LBLMAP_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif /* !__cplusplus */

typedef struct lblmap_ent_t
{
  char *key;
  uint64_t val;
  struct lblmap_ent_t *next;
} lblmap_ent_t;

typedef struct lblmap_t
{
  size_t bucket_size;
  lblmap_ent_t **ptr;
} lblmap_t;

#ifdef __cplusplus
extern "C"
{
#endif				/* !__cplusplus */

  extern lblmap_t init_map (size_t bucket_size);

  extern void put_entry (lblmap_t * map, char *key, uint64_t val);

  extern bool has_key (lblmap_t * map, char *key);

  extern void free_map (lblmap_t * map);

#ifdef __cplusplus
}
#endif				/* !__cplusplus */

#endif				/* !__LBLMAP_H__ */
