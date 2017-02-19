%{
#include "rasm.h"
#include "bcode.h"
#include "lblmap.h"
#include "instrbuf.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif /* !__cplusplus */

typedef enum section_t
{
  TEXT = 0, DATA
} section_t;

typedef struct trunit_t
{
  uint64_t stack_size;
  uint64_t estack_size;
  lblmap_t lmap;
  instrbuf_t ibuf;
} trunit_t;

#ifdef __cplusplus
extern "C"
{
#endif /* !__cplusplus */

  extern void yyerror (char *s);

  extern int yylex ();

  extern int yyparse ();

  extern uint64_t get_lbl_addr (bool pref_data, char *str);

  extern FILE *yyin;

  extern int line_num;

  extern size_t pass;

  extern lblmap_t glmap;

  extern opcode_t opc;

  extern FILE *ropool;

  extern uint64_t code_len;

  extern uint64_t pool_len;

  extern trunit_t *trans_unit;

  extern size_t tunit_idx;

  extern section_t section;

#ifdef __cplusplus
}
#endif /* !__cplusplus */
%}

/**
 * The RASM assembler is a three-passed assembler.
 *
 * 0 - Handle labels address and generate constant pool
 * 1 - Handle labels visibility (and some collisions)
 * 2 - Handle label collisions and generate code
 */

%token COLON COMMA
%token D_GLOBAL D_SECTION D_STACK D_ESTACK S_TEXT S_DATA S_STDOUT S_STDERR S_STDIN S_DB S_DW S_DD S_DQ K_HALT K_MOV K_MH32 K_ML32 K_ML16 K_ML8 K_SWP K_I2F K_B2F K_F2IF K_F2B K_F2IC K_RMEH K_THROW K_PUSH K_POP K_LDEX K_PLDEX K_ADD K_SUB K_MUL K_DIV K_MOD K_AND K_OR K_XOR K_NOT K_LSH K_RSH K_SRSH K_ROL K_ROR K_CALL K_JMP K_RET K_JE K_JL K_JG K_JLS K_JGS K_JOF K_JZ K_INEH K_LDS K_STS K_STFBS K_ALLOC K_FREE K_LDB K_LDW K_LDD K_LDQ K_STB K_STW K_STD K_STQ K_SJE K_SJL K_SJSL K_SJG K_SJSG K_SJZ K_LDC K_FOPEN K_FCLOSE K_FFLUSH K_FREWIND K_FREAD K_FWRTB K_FWRTQ K_FWRTS

%union
{
  uint64_t ival;
  char *sval;
}

%token <sval> LABEL STR
%token <ival> INT IREG FREG

%%

prog:
    prog section
    | section
    ;

section:
    D_SECTION S_TEXT { section = TEXT; } code
    | D_SECTION S_DATA { section = DATA; } data
    | visDirectives
    ;

data:
    data bytes
    | bytes
    ;

bytes:
    defLabel
    | cbytes
    ;

cbytes:
    cbytes COMMA cbytes_p
    | cbytes_p
    ;

cbytes_p:
    S_DB INT {
      if (pass == 0)
	putc ($2, ropool);
    }
    | S_DW INT {
      if (pass == 0)
	{
	  const uint16_t tmp = $2;
	  fwrite (&tmp, sizeof (char), 2, ropool);
	}
    }
    | S_DD INT {
      if (pass == 0)
	{
	  const uint32_t tmp = $2;
	  fwrite (&tmp, sizeof (char), 4, ropool);
	}
    }
    | S_DQ INT {
      if (pass == 0)
	{
	  const uint64_t tmp = $2;
	  fwrite (&tmp, sizeof (char), 8, ropool);
	}
    }
    | STR {
      if (pass == 0)
	{
	  fprintf (ropool, $1);
	  putc (0, ropool);
	}
      free ($1);
    }
    ;

code:
    code stmt
    | stmt
    ;

stmt:
    defLabel
    | visDirectives
    | visInstr {
      ++code_len;
      if (pass == 2)
	push (&trans_unit[tunit_idx].ibuf, opc);
    }
    ;

defLabel:
    LABEL COLON	{
      switch (pass)
	{
	case 0:
	  if (has_key (&trans_unit[tunit_idx].lmap, $1))
	    yyerror ("Label redefined!");
	  switch (section)
	    {
	    case TEXT:
	      put_entry (&trans_unit[tunit_idx].lmap, $1, code_len, tunit_idx);
	      break;
	    case DATA:
	      fflush (ropool);
	      put_entry (&trans_unit[tunit_idx].lmap, $1, pool_len, tunit_idx);
	      set_data_flag (&trans_unit[tunit_idx].lmap, $1, true);
	      break;
	    }
	  break;
	case 1:
	  break;
	case 2:
	  if (has_key (&glmap, $1))
	    if (get_trans_unit (&glmap, $1) != tunit_idx)
	      yyerror ("Label collides with global label");
	  break;
	}
    }
    ;

visDirectives:
    D_GLOBAL LABEL {
      switch (pass)
	{
	case 0:
	  break;
	case 1:
	  {
	    lblmap_ent_t *oldcell = remove_entry (&trans_unit[tunit_idx].lmap, $2);
	    if (oldcell == NULL)
	      yyerror ("Attempt to export undefined label");
	    if (!put_if_empty (&glmap, oldcell))
	      yyerror ("Label collides with global label");
	    break;
	  }
	case 2:
	  break;
	}
    }
    | D_STACK INT {
      trans_unit[tunit_idx].stack_size = $2;
    }
    | D_ESTACK INT {
      trans_unit[tunit_idx].estack_size = $2;
    }
    ;

visInstr:
    K_HALT IREG {
      opc = RLVM_HALT ($2);
    }
    | K_MOV IREG COMMA IREG {
      opc = RLVM_IRMV64 ($2, $4);
    }
    | K_MH32 IREG COMMA IREG {
      opc = RLVM_IRMVH32 ($2, $4);
    }
    | K_ML32 IREG COMMA IREG {
      opc = RLVM_IRMVL32 ($2, $4);
    }
    | K_ML16 IREG COMMA IREG {
      opc = RLVM_IRMVL16 ($2, $4);
    }
    | K_ML8 IREG COMMA IREG {
      opc = RLVM_IRMVL8 ($2, $4);
    }
    | K_MOV FREG COMMA FREG {
      opc = RLVM_FRMV ($2, $4);
    }
    | K_SWP IREG COMMA IREG {
      opc = RLVM_IRSWP ($2, $4);
    }
    | K_I2F FREG COMMA IREG {
      opc = RLVM_IRTFR ($2, $4, 0);
    }
    | K_B2F FREG COMMA IREG {
      opc = RLVM_IRTFR ($2, $4, 1);
    }
    | K_F2IF IREG COMMA FREG {
      opc = RLVM_FRTIR ($2, $4, 0);
    }
    | K_F2B IREG COMMA FREG {
      opc = RLVM_FRTIR ($2, $4, 1);
    }
    | K_F2IC IREG COMMA FREG {
      opc = RLVM_FRTIR ($2, $4, 2);
    }
    | K_RMEH {
      opc = RLVM_RMEH ();
    }
    | K_THROW IREG {
      opc = RLVM_THROW ($2);
    }
    | K_PUSH IREG {
      opc = RLVM_PUSH1 ($2);
    }
    | K_PUSH IREG COMMA IREG {
      opc = RLVM_PUSH2 ($2, $4);
    }
    | K_PUSH IREG COMMA IREG COMMA IREG {
      opc = RLVM_PUSH3 ($2, $4, $6);
    }
    | K_POP IREG {
      opc = RLVM_POP1 ($2);
    }
    | K_POP IREG COMMA IREG {
      opc = RLVM_POP2 ($2, $4);
    }
    | K_POP IREG COMMA IREG COMMA IREG {
      opc = RLVM_POP3 ($2, $4, $6);
    }
    | K_LDEX {
      opc = RLVM_LDEX (0, 1);
    }
    | K_LDEX IREG {
      opc = RLVM_LDEX ($2, 0);
    }
    | K_PLDEX IREG {
      opc = RLVM_LDEX ($2, 2);
    }
    | K_ADD IREG COMMA IREG COMMA IREG {
      opc = RLVM_ADD ($2, $4, $6, 0, 0);
    }
    | K_ADD IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_ADD ($2, $4, $6, 1, $9);
    }
    | K_ADD IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_ADD ($2, $4, $6, 2, $9);
    }
    | K_ADD IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_ADD ($2, $4, $6, 3, $9);
    }
    | K_SUB IREG COMMA IREG COMMA IREG {
      opc = RLVM_SUB ($2, $4, $6, 0, 0);
    }
    | K_SUB IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_SUB ($2, $4, $6, 1, $9);
    }
    | K_SUB IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_SUB ($2, $4, $6, 2, $9);
    }
    | K_SUB IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_SUB ($2, $4, $6, 3, $9);
    }
    | K_MUL IREG COMMA IREG COMMA IREG {
      opc = RLVM_MUL ($2, $4, $6, 0, 0);
    }
    | K_MUL IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_MUL ($2, $4, $6, 1, $9);
    }
    | K_MUL IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_MUL ($2, $4, $6, 2, $9);
    }
    | K_MUL IREG COMMA IREG COMMA IREG COMMA K_SRSH INT  {
      opc = RLVM_MUL ($2, $4, $6, 3, $9);
    }
    | K_DIV IREG COMMA IREG COMMA IREG {
      opc = RLVM_DIV ($2, $4, $6, 0, 0);
    }
    | K_DIV IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_DIV ($2, $4, $6, 1, $9);
    }
    | K_DIV IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_DIV ($2, $4, $6, 2, $9);
    }
    | K_DIV IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_DIV ($2, $4, $6, 3, $9);
    }
    | K_MOD IREG COMMA IREG COMMA IREG {
      opc = RLVM_MOD ($2, $4, $6, 0, 0);
    }
    | K_MOD IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_MOD ($2, $4, $6, 1, $9);
    }
    | K_MOD IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_MOD ($2, $4, $6, 2, $9);
    }
    | K_MOD IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_MOD ($2, $4, $6, 3, $9);
    }
    | K_AND IREG COMMA IREG COMMA IREG {
      opc = RLVM_AND ($2, $4, $6, 0, 0);
    }
    | K_AND IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_AND ($2, $4, $6, 1, $9);
    }
    | K_AND IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_AND ($2, $4, $6, 2, $9);
    }
    | K_AND IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_AND ($2, $4, $6, 3, $9);
    }
    | K_OR IREG COMMA IREG COMMA IREG {
      opc = RLVM_OR ($2, $4, $6, 0, 0);
    }
    | K_OR IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_OR ($2, $4, $6, 1, $9);
    }
    | K_OR IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_OR ($2, $4, $6, 2, $9);
    }
    | K_OR IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_OR ($2, $4, $6, 3, $9);
    }
    | K_XOR IREG COMMA IREG COMMA IREG {
      opc = RLVM_XOR ($2, $4, $6, 0, 0);
    }
    | K_XOR IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_XOR ($2, $4, $6, 1, $9);
    }
    | K_XOR IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_XOR ($2, $4, $6, 2, $9);
    }
    | K_XOR IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_XOR ($2, $4, $6, 3, $9);
    }
    | K_NOT IREG COMMA IREG {
      opc = RLVM_NOT ($2, $4, 0, 0);
    }
    | K_NOT IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_NOT ($2, $4, 1, $7);
    }
    | K_NOT IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_NOT ($2, $4, 2, $7);
    }
    | K_NOT IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_NOT ($2, $4, 3, $7);
    }
    | K_LSH IREG COMMA IREG COMMA IREG {
      opc = RLVM_LSH ($2, $4, $6, 0, 0);
    }
    | K_LSH IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_LSH ($2, $4, $6, 1, $9);
    }
    | K_LSH IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_LSH ($2, $4, $6, 2, $9);
    }
    | K_LSH IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_LSH ($2, $4, $6, 3, $9);
    }
    | K_RSH IREG COMMA IREG COMMA IREG {
      opc = RLVM_RSH ($2, $4, $6, 0, 0);
    }
    | K_RSH IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_RSH ($2, $4, $6, 1, $9);
    }
    | K_RSH IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_RSH ($2, $4, $6, 2, $9);
    }
    | K_RSH IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_RSH ($2, $4, $6, 3, $9);
    }
    | K_SRSH IREG COMMA IREG COMMA IREG {
      opc = RLVM_SRSH ($2, $4, $6, 0, 0);
    }
    | K_SRSH IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_SRSH ($2, $4, $6, 1, $9);
    }
    | K_SRSH IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_SRSH ($2, $4, $6, 2, $9);
    }
    | K_SRSH IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_SRSH ($2, $4, $6, 3, $9);
    }
    | K_ROL IREG COMMA IREG COMMA IREG {
      opc = RLVM_ROL ($2, $4, $6, 0, 0);
    }
    | K_ROL IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_ROL ($2, $4, $6, 1, $9);
    }
    | K_ROL IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_ROL ($2, $4, $6, 2, $9);
    }
    | K_ROL IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_ROL ($2, $4, $6, 3, $9);
    }
    | K_ROR IREG COMMA IREG COMMA IREG {
      opc = RLVM_ROR ($2, $4, $6, 0, 0);
    }
    | K_ROR IREG COMMA IREG COMMA IREG COMMA K_LSH INT {
      opc = RLVM_ROR ($2, $4, $6, 1, $9);
    }
    | K_ROR IREG COMMA IREG COMMA IREG COMMA K_RSH INT {
      opc = RLVM_ROR ($2, $4, $6, 2, $9);
    }
    | K_ROR IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
      opc = RLVM_ROR ($2, $4, $6, 3, $9);
    }
    | K_ADD FREG COMMA FREG COMMA FREG {
      opc = RLVM_ADDF ($2, $4, $6);
    }
    | K_SUB FREG COMMA FREG COMMA FREG {
      opc = RLVM_SUBF ($2, $4, $6);
    }
    | K_MUL FREG COMMA FREG COMMA FREG {
      opc = RLVM_MULF ($2, $4, $6);
    }
    | K_DIV FREG COMMA FREG COMMA FREG {
      opc = RLVM_DIVF ($2, $4, $6);
    }
    | K_MOD FREG COMMA FREG COMMA FREG {
      opc = RLVM_MODF ($2, $4, $6);
    }
    | K_MOV IREG COMMA INT {
      opc = RLVM_IRLDI ($2, 0, $4);
    }
    | K_MOV IREG COMMA INT COMMA K_LSH INT {
      opc = RLVM_IRLDI ($2, $7, $4);
    }
    | K_ADD IREG COMMA IREG COMMA INT {
      opc = RLVM_ADDI ($2, $4, $6);
    }
    | K_SUB IREG COMMA IREG COMMA INT {
      opc = RLVM_SUBI ($2, $4, $6);
    }
    | K_MUL IREG COMMA IREG COMMA INT {
      opc = RLVM_MULI ($2, $4, $6);
    }
    | K_DIV IREG COMMA IREG COMMA INT {
      opc = RLVM_DIVI ($2, $4, $6);
    }
    | K_MOD IREG COMMA IREG COMMA INT {
      opc = RLVM_MODI ($2, $4, $6);
    }
    | K_AND IREG COMMA IREG COMMA INT {
      opc = RLVM_ANDI ($2, $4, $6);
    }
    | K_OR IREG COMMA IREG COMMA INT {
      opc = RLVM_ORI ($2, $4, $6);
    }
    | K_XOR IREG COMMA IREG COMMA INT {
      opc = RLVM_XORI ($2, $4, $6);
    }
    | K_CALL LABEL {
      if (pass == 2)
	opc = RLVM_CALL (get_lbl_addr (false, $2));
    }
    | K_JMP LABEL {
      if (pass == 2)
	opc = RLVM_JMP (get_lbl_addr (false, $2));
    }
    | K_RET {
      opc = RLVM_RET ();
    }
    | K_JE IREG COMMA IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JE ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JL IREG COMMA IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JL ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JG IREG COMMA IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JG ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JLS IREG COMMA IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JLS ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JGS IREG COMMA IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JGS ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JE FREG COMMA FREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JFE ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JL FREG COMMA FREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JFL ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JG FREG COMMA FREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JFG ($2, $4, get_lbl_addr (false, $6));
    }
    | K_JOF INT {
      opc = RLVM_JOF ($2);
    }
    | K_JMP IREG {
      opc = RLVM_JIR ($2, 0, 0);
    }
    | K_JMP IREG COMMA K_LSH INT {
      opc = RLVM_JIR ($2, $5, 0);
    }
    | K_JMP IREG COMMA K_LSH INT COMMA INT {
      opc = RLVM_JIR ($2, $5, $7);
    }
    | K_JZ IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JIRZ ($2, get_lbl_addr (false, $4));
    }
    | K_JZ FREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_JFRZ ($2, get_lbl_addr (false, $4));
    }
    | K_INEH LABEL {
      if (pass == 2)
	opc = RLVM_INEH (get_lbl_addr (false, $2));
    }
    | K_LDS IREG COMMA INT {
      opc = RLVM_IRLD ($2, $4);
    }
    | K_LDS FREG COMMA INT {
      opc = RLVM_FRLD ($2, $4);
    }
    | K_STS IREG COMMA INT {
      opc = RLVM_IRST ($2, $4);
    }
    | K_STS FREG COMMA INT {
      opc = RLVM_FRST ($2, $4);
    }
    | K_STFBS FREG COMMA INT {
      opc = RLVM_FBST ($2, $4);
    }
    | K_ALLOC IREG COMMA INT {
      opc = RLVM_ALLOCI ($2, $4);
    }
    | K_ALLOC IREG COMMA IREG {
      opc = RLVM_IRALLOC ($2, $4);
    }
    | K_FREE IREG {
      opc = RLVM_FREE ($2);
    }
    | K_LDB IREG COMMA IREG COMMA INT {
      opc = RLVM_LDB ($2, $4, $6);
    }
    | K_LDW IREG COMMA IREG COMMA INT {
      opc = RLVM_LDW ($2, $4, $6);
    }
    | K_LDD IREG COMMA IREG COMMA INT {
      opc = RLVM_LDD ($2, $4, $6);
    }
    | K_LDQ IREG COMMA IREG COMMA INT {
      opc = RLVM_LDQ ($2, $4, $6);
    }
    | K_STB IREG COMMA IREG COMMA INT {
      opc = RLVM_STB ($2, $4, $6);
    }
    | K_STW IREG COMMA IREG COMMA INT {
      opc = RLVM_STW ($2, $4, $6);
    }
    | K_STD IREG COMMA IREG COMMA INT {
      opc = RLVM_STD ($2, $4, $6);
    }
    | K_STQ IREG COMMA IREG COMMA INT {
      opc = RLVM_STQ ($2, $4, $6);
    }
    | K_SJE IREG COMMA IREG {
      opc = RLVM_SIREQ ($2, $4);
    }
    | K_SJE FREG COMMA FREG {
      opc = RLVM_SFREQ ($2, $4);
    }
    | K_SJL IREG COMMA IREG {
      opc = RLVM_SIRL ($2, $4);
    }
    | K_SJL FREG COMMA FREG {
      opc = RLVM_SFRL ($2, $4);
    }
    | K_SJSL IREG COMMA IREG {
      opc = RLVM_SIRSL ($2, $4);
    }
    | K_SJG IREG COMMA IREG {
      opc = RLVM_SIRG ($2, $4);
    }
    | K_SJG FREG COMMA FREG {
      opc = RLVM_SFRG ($2, $4);
    }
    | K_SJSG IREG COMMA IREG {
      opc = RLVM_SIRSG ($2, $4);
    }
    | K_SJZ IREG {
      opc = RLVM_SIRZ ($2);
    }
    | K_SJZ FREG {
      opc = RLVM_SFRZ ($2);
    }
    | K_LDC IREG COMMA IREG COMMA INT {
      if (pass == 2)
	opc = RLVM_LDPO ($2, $4, $6);
    }
    | K_LDC IREG COMMA LABEL {
      if (pass == 2)
	opc = RLVM_LDPA ($2, get_lbl_addr (true, $4));
    }
    | K_FREAD IREG COMMA IREG {
      opc = RLVM_FREAD_CH ($2, $4);
    }
    | K_FREAD IREG COMMA IREG COMMA IREG {
      opc = RLVM_FREAD_I64 ($2, $4, $6);
    }
    | K_FREAD FREG COMMA IREG COMMA IREG {
      opc = RLVM_FREAD_F64 ($2, $4, $6);
    }
    | K_LDC IREG COMMA S_STDOUT {
      opc = RLVM_LD_STDOUT ($2);
    }
    | K_LDC IREG COMMA S_STDERR {
      opc = RLVM_LD_STDERR ($2);
    }
    | K_LDC IREG COMMA S_STDIN {
      opc = RLVM_LD_STDIN ($2);
    }
    | K_FOPEN IREG COMMA IREG COMMA IREG {
      opc = RLVM_FOPEN ($2, $4, $6);
    }
    | K_FCLOSE IREG COMMA IREG {
      opc = RLVM_FCLOSE ($2, $4);
    }
    | K_FFLUSH IREG COMMA IREG {
      opc = RLVM_FFLUSH ($2, $4);
    }
    | K_FREWIND IREG {
      opc = RLVM_FREWIND ($2);
    }
    | K_FWRTB IREG COMMA IREG COMMA IREG {
      opc = RLVM_FWRITE_CH ($2, $4, $6);
    }
    | K_FWRTQ IREG COMMA IREG COMMA IREG {
      opc = RLVM_FWRITE_I64 ($2, $4, $6);
    }
    | K_FWRTQ IREG COMMA IREG COMMA FREG {
      opc = RLVM_FWRITE_F64 ($2, $4, $6);
    }
    | K_FWRTS IREG COMMA IREG COMMA IREG {
      opc = RLVM_FWRITE_STR ($2, $4, $6);
    }
    ;

%%

/* No assignment. Only allocate memory */
size_t pass;
lblmap_t glmap;
opcode_t opc;
FILE *ropool;
trunit_t *trans_unit;
size_t tunit_idx;
uint64_t code_len;
uint64_t pool_len;
section_t section;

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
assemble (FILE **in, size_t count)
{
  char *pool_dat;
  ropool = open_memstream (&pool_dat, &pool_len);

  trans_unit = calloc (count, sizeof (trunit_t));
  size_t i;
  for (i = 0; i < count; ++i)
    trans_unit[i] = (trunit_t) {
      .stack_size = 0,
      .estack_size = 0,
      .lmap = init_map (64),
      .ibuf = init_buf (16)
    };

  code_len = pass = 0;
  glmap = init_map (64);

  for (tunit_idx = 0; tunit_idx < count; ++tunit_idx)
    {
      section = TEXT;
      yyin = in[tunit_idx];

      do
	{
	  yyparse ();
	}
      while (!feof (yyin));

      rewind (yyin);
      line_num = 1;
    }
  ++pass, code_len = 0;

  for (tunit_idx = 0; tunit_idx < count; ++tunit_idx)
    {
      yyin = in[tunit_idx];
      do
	{
	  yyparse ();
	}
      while (!feof (yyin));

      rewind (yyin);
      line_num = 1;
    }
  ++pass, code_len = 0;

  for (tunit_idx = 0; tunit_idx < count; ++tunit_idx)
    {
      yyin = in[tunit_idx];
      do
	{
	  yyparse ();
	}
      while (!feof (yyin));
      line_num = 1;
    }

  fflush (ropool);
  fclose (ropool);

  size_t final_cssize = 0;
  size_t final_essize = 0;

  for (i = 0; i < count; ++i)
    {
      final_cssize += trans_unit[i].stack_size;
      final_essize += trans_unit[i].estack_size;
    }

  bcode_t obj = (bcode_t) {
    .magic = {
      0x2C, is_big_endian () ? 0xDF : 0xD0
    },
    .cstack_size = final_cssize,
    .estack_size = final_essize,
    .ropool_size = pool_len,
    .code_size = code_len,
    .code = calloc (sizeof (opcode_t), code_len),
    .ropool = pool_dat /* DO NOT FREE ropool_dat! */
  };

  size_t off;
  for (i = off = 0; i < count; ++i)
    {
      memcpy (obj.code + off, trans_unit[i].ibuf.ptr,
	      trans_unit[i].ibuf.size * sizeof (opcode_t));
      off += trans_unit[i].ibuf.size;
    }

  free_map (&glmap);
  for (i = 0; i < count; ++i)
    {
      free_map (&trans_unit[i].lmap);
      free_buf (&trans_unit[i].ibuf);
    }
  free (trans_unit);

  return obj;
}

void
yyerror (char *s)
{
  fprintf (stderr, "%s (at line %d)\n", s, line_num);
  exit (-1);
}

uint64_t
get_lbl_addr (bool pref_data, char *str)
{
  if (has_key (&trans_unit[tunit_idx].lmap, str))
    {
      if (get_data_flag (&trans_unit[tunit_idx].lmap, str))
	{
	  if (!pref_data)
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	}
      else
	{
	  if (pref_data)
	    fprintf (stderr, "Warning: Using non-data label (at line %d)\n",
		     line_num);
	}
      return get_val (&trans_unit[tunit_idx].lmap, str);
    }
  if (has_key (&glmap, str))
    {
      if (get_data_flag (&glmap, str))
	{
	  if (!pref_data)
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	}
      else
	{
	  if (pref_data)
	    fprintf (stderr, "Warning: Using non-data label (at line %d)\n",
		     line_num);
	}
      return get_val (&glmap, str);
    }
  yyerror ("Undefined label");
  return 0;			/* Never reaches this line */
}
