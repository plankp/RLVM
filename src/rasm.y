%{
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

  extern uint64_t roaddr;

  extern uint64_t stack_size;

  extern uint64_t estack_size;

  extern FILE *ropool;

  extern lblmap_t lmap;

  extern instrbuf_t ibuf;

  extern opcode_t opc;

  extern section_t section;

#ifdef __cplusplus
}
#endif /* !__cplusplus */
%}

%token COLON COMMA
%token D_GLOBAL D_SECTION D_STACK D_ESTACK S_TEXT S_DATA K_HALT K_MOV K_MH32 K_ML32 K_ML16 K_ML8 K_SWP K_I2F K_B2F K_F2IF K_F2B K_F2IC K_RMEH K_THROW K_PUSH K_POP K_LDEX K_PLDEX K_ADD K_SUB K_MUL K_DIV K_MOD K_AND K_OR K_XOR K_NOT K_LSH K_RSH K_SRSH K_ROL K_ROR K_CALL K_JMP K_RET K_JE K_JL K_JG K_JLS K_JGS K_JOF K_JZ K_INEH K_LDS K_STS K_STFBS K_ALLOC K_FREE K_LDB K_LDW K_LDD K_LDQ K_STB K_STW K_STD K_STQ K_SJE K_SJL K_SJSL K_SJG K_SJSG K_SJZ K_LDC

%union
{
  int ival;
  char cval;
  char *sval;
}

%token <sval> LABEL
%token <ival> INT IREG FREG
%token <cval> CHAR

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
    cbytes COMMA CHAR {
      ++roaddr;
      putc ($3, ropool);
    }
    | CHAR {
      ++roaddr;
      putc ($1, ropool);
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
      ++addr;
      if (pass != 0)
	push (&ibuf, opc);
    }
    ;

defLabel:
    LABEL COLON	{
      /* Only do something in pass 0 */
      if (pass == 0)
	{
	  if (has_key (&lmap, $1))
	    yyerror ("Label redefined!");
	  switch (section)
	    {
	    case TEXT:
	      put_entry (&lmap, $1, addr);
	      break;
	    case DATA:
	      put_entry (&lmap, $1, roaddr);
	      break;
	    }
	}
    }
    ;

visDirectives:
    D_GLOBAL LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $2))
	    yyerror ("Attempt to export undefined label");
	  set_global_flag (&lmap, $2, true);
	}
    }
    | D_STACK INT {
      stack_size = $2;
    }
    | D_ESTACK INT {
      estack_size = $2;
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
    | K_MUL IREG COMMA IREG COMMA IREG COMMA K_SRSH INT {
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
    | K_CALL INT {
      opc = RLVM_CALL ($2);
    }
    | K_CALL LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $2))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $2))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_CALL (get_val (&lmap, $2));
	}
    }
    | K_JMP INT {
      opc = RLVM_JMP ($2);
    }
    | K_JMP LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $2))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $2))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JMP (get_val (&lmap, $2));
	}
    }
    | K_RET {
      opc = RLVM_RET ();
    }
    | K_JE IREG COMMA IREG COMMA INT {
      opc = RLVM_JE ($2, $4, $6);
    }
    | K_JE IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JE ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JL IREG COMMA IREG COMMA INT {
      opc = RLVM_JL ($2, $4, $6);
    }
    | K_JL IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JL ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JG IREG COMMA IREG COMMA INT {
      opc = RLVM_JG ($2, $4, $6);
    }
    | K_JG IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JG ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JLS IREG COMMA IREG COMMA INT {
      opc = RLVM_JLS ($2, $4, $6);
    }
    | K_JLS IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JLS ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JGS IREG COMMA IREG COMMA INT {
      opc = RLVM_JGS ($2, $4, $6);
    }
    | K_JGS IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JGS ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JE FREG COMMA FREG COMMA INT {
      opc = RLVM_JFE ($2, $4, $6);
    }
    | K_JE FREG COMMA FREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JFE ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JL FREG COMMA FREG COMMA INT {
      opc = RLVM_JFL ($2, $4, $6);
    }
    | K_JL FREG COMMA FREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JFL ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_JG FREG COMMA FREG COMMA INT {
      opc = RLVM_JFG ($2, $4, $6);
    }
    | K_JG FREG COMMA FREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JFG ($2, $4, get_val (&lmap, $6));
	}
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
    | K_JZ IREG COMMA INT {
      opc = RLVM_JIRZ ($2, $4);
    }
    | K_JZ IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $4))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $4))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JIRZ ($2, get_val (&lmap, $4));
	}
    }
    | K_JZ FREG COMMA INT {
      opc = RLVM_JFRZ ($2, $4);
    }
    | K_JZ FREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $4))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $4))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_JFRZ ($2, get_val (&lmap, $4));
	}
    }
    | K_INEH INT {
      opc = RLVM_INEH ($2);
    }
    | K_INEH LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $2))
	    yyerror ("Undefined label");
	  if (get_data_flag (&lmap, $2))
	    fprintf (stderr, "Warning: Using data label (at line %d)\n",
		     line_num);
	  opc = RLVM_INEH (get_val (&lmap, $2));
	}
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
    | K_LDC IREG COMMA IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $6))
	    yyerror ("Undefined label");
	  if (!get_data_flag (&lmap, $6))
	    fprintf (stderr, "Warning: Using non-data label (at line %d)\n",
		     line_num);
	  opc = RLVM_LDPO ($2, $4, get_val (&lmap, $6));
	}
    }
    | K_LDC IREG COMMA IREG COMMA INT {
      opc = RLVM_LDPO ($2, $4, $6);
    }
    | K_LDC IREG COMMA LABEL {
      if (pass != 0)
	{
	  if (!has_key (&lmap, $4))
	    yyerror ("Undefined label");
	  if (!get_data_flag (&lmap, $4))
	    fprintf (stderr, "Warning: Using non-data label (at line %d)\n",
		     line_num);
	  opc = RLVM_LDPA ($2, get_val (&lmap, $4));
	}
    }
    | K_LDC IREG COMMA INT {
      opc = RLVM_LDPA ($2, $4);
    }
    ;

%%

/* No assignment. Only allocate memory */
size_t pass;
uint64_t addr;
uint64_t roaddr;
uint64_t stack_size;
uint64_t estack_size;
FILE *ropool;
lblmap_t lmap;
instrbuf_t ibuf;
opcode_t opc;
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
assemble (FILE *in)
{
  pass = stack_size = estack_size = roaddr = addr = 0;
  section = TEXT;
  lmap = init_map (64);
  ibuf = init_buf (16);
  yyin = in;

  char *ropool_dat;
  size_t ropool_len;
  ropool = open_memstream (&ropool_dat, &ropool_len);

  do
    {
      yyparse ();
    }
  while (!feof (yyin));

  rewind (yyin);
  ++pass, roaddr = addr = 0;

  do
    {
      yyparse ();
    }
  while (!feof (yyin));

  fflush (ropool);
  fclose (ropool);

  bcode_t obj = (bcode_t) {
    .magic = {
      0x2C, is_big_endian () ? 0xDF : 0xD0
    },
    .cstack_size = stack_size,
    .estack_size = estack_size,
    .ropool_size = ropool_len,
    .code_size = ibuf.size,
    .code = calloc (sizeof (opcode_t), ibuf.size),
    .ropool = ropool_dat		/* DO NOT FREE ropool_dat! */
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
