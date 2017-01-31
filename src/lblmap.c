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

#include "lblmap.h"

static inline uint64_t
__djb2_hash (char *str)
{
  uint64_t hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c;	/* hash * 33 + c */

  return hash;
}

lblmap_t
init_map (size_t _bucket_size)
{
  return (lblmap_t)
  {
  .bucket_size = _bucket_size,.ptr =
      calloc (sizeof (lblmap_ent_t), _bucket_size)};
}

static inline lblmap_ent_t **
__cell_find (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = map->ptr + (__djb2_hash (key) % map->bucket_size);
cell_relloc:
  if (*cell == NULL)
    return cell;
  if (strcmp ((*cell)->key, key) != 0)
    {
      cell = &(*cell)->next;
      goto cell_relloc;
    }
  return cell;
}

void
put_entry (lblmap_t * map, char *key, uint64_t val, size_t tunit)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell == NULL)
    {
      /* Insert here, cell was not occupied */
      *cell = calloc (sizeof (lblmap_ent_t), 1);
      (*cell)->key = key;
      (*cell)->val = val;
      (*cell)->trans_unit = tunit;
      (*cell)->data_flag = false;
      (*cell)->next = NULL;
    }
  else
    {
      /* Overwrite since same key */
      (*cell)->val = val;
    }
}

bool
put_if_empty (lblmap_t * map, lblmap_ent_t * ent)
{
  lblmap_ent_t **cell = __cell_find (map, ent->key);
  if (*cell == NULL)
    {
      *cell = ent;
      return true;
    }
  return false;
}

bool
has_key (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell == NULL)
    return false;
  return true;
}

void
set_data_flag (lblmap_t * map, char *key, bool flag)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell != NULL)
    (*cell)->data_flag = flag;
}

size_t
get_trans_unit (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell != NULL)
    return (*cell)->trans_unit;
  return 0;
}

bool
get_data_flag (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell != NULL)
    return (*cell)->data_flag;
  return false;
}

uint64_t
get_val (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = __cell_find (map, key);
  if (*cell == NULL)
    return 0;
  return (*cell)->val;
}

static inline void
__free_entry (lblmap_ent_t * ent)
{
  free (ent->key);
  if (ent->next != NULL)
    {
      __free_entry (ent->next);
      free (ent->next);
      ent->next = NULL;
    }
}

lblmap_ent_t *
remove_entry (lblmap_t * map, char *key)
{
  lblmap_ent_t **cell = map->ptr + (__djb2_hash (key) % map->bucket_size);
cell_relloc:
  if (*cell == NULL)
    return NULL;
  if (strcmp ((*cell)->key, key) != 0)
    {
      cell = &(*cell)->next;
      goto cell_relloc;
    }
  lblmap_ent_t *old = *cell;
  *cell = (*cell)->next;
  old->next = NULL;
  return old;
}

void
free_map (lblmap_t * map)
{
  size_t i;
  for (i = 0; i < map->bucket_size; ++i)
    {
      if (map->ptr[i] != NULL)
	{
	  __free_entry (map->ptr[i]);
	  free (map->ptr[i]);
	}
    }
  free (map->ptr);
  map->ptr = NULL;
  map->bucket_size = 0;
}
