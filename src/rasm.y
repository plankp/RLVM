%{
#include "bcode.h"
#include "lblmap.h"
#include "instrbuf.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif /* !__cplusplus */

  extern void yyerror (char *s);

  extern int yylex ();

  extern int yyparse ();

  extern bcode_t assemble (FILE *in);

  extern FILE *yyin;

  extern int line_num;

  extern size_t pass;

  extern uint64_t addr;

  extern lblmap_t lmap;

  extern instrbuf_t ibuf;

  extern opcode_t opc;

#ifdef __cplusplus
}
#endif /* !__cplusplus */
%}

%token COLON COMMA
%token K_HALT

%union
{
  int ival;
  char *sval;
}

%token <sval> LABEL
%token <ival> INT IREG FREG

%%

prog:
    prog stmt
    | stmt
    ;

stmt:
    defLabel
    | visInstr	{
  ++addr;
  if (pass != 0)
    {
      push (&ibuf, opc);
    }
}
    ;

defLabel:
    LABEL COLON	{
  /* Only do something in pass 0 */
  /* Store this inside a hashmap. Done this before! */
  if (pass == 0)
    {
      if (has_key (&lmap, $1))
        {
          yyerror ("Label redefined!");
        }
      put_entry (&lmap, $1, addr);
    }
}
    ;

visInstr:
    K_HALT IREG {
  opc = RLVM_HALT ($2);
}
    ;

%%

/* No assignment. Only allocate memory */
size_t pass;
uint64_t addr;
lblmap_t lmap;
instrbuf_t ibuf;
opcode_t opc;

static inline
int
is_big_endian (void)
{
  union {
    uint32_t i;
    char c[sizeof (uint32_t)];
  } bint = { 0x01020304 };
  return bint.c[0] == 1;
}

bcode_t
assemble (FILE *in)
{
  pass = addr = 0;
  lmap = init_map (64);
  ibuf = init_buf (16);
  yyin = in;

  do
    {
      yyparse ();
    }
  while (!feof (yyin));

  rewind (yyin);
  ++pass, addr = 0;
  
  do
    {
      yyparse ();
    }
  while (!feof (yyin));

  bcode_t obj = (bcode_t) {
    .magic = {
      0x2C, is_big_endian () ? 0xDF : 0xD0
    },
    .cstack_size = 10, // TODO: THIS
    .estack_size = 10, // TODO: THIS
    .code_size = ibuf.size,
    .code = calloc (sizeof (opcode_t), ibuf.size)
  };
  memcpy (obj.code, ibuf.ptr, ibuf.size * sizeof (opcode_t));

  free_map (&lmap);
  free_buf (&ibuf);

  return obj;
}

void
yyerror (char *s)
{
  fprintf (stderr, "%s (at line %d)\n", s, line_num);
  exit (-1);
}
