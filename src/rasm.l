%{
#include "rasm.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif /* !__cplusplus */

  extern void yyerror (char *s);

  int line_num = 1;

#ifdef __cplusplus
}
#endif /* !__cplusplus */
%}

%option noyywrap

%%

[ \t\r]				;
\n				++line_num;
:				return COLON;
,				return COMMA;
(fp|FP)([0-9]|[12][0-9]|3[01])	{
  yylval.ival = atoi (yytext + 2);
  return FREG;
				}
[rR]([0-9]|[12][0-9]|3[01])	{
  yylval.ival = atoi (yytext + 1);
  return IREG;
				}
0(b|B)[01]+			{
  yylval.ival = strtol (yytext + 2, NULL, 2);
  return INT;
				}
0(x|X)[a-fA-F0-9]+		{
  yylval.ival = strtol (yytext + 2, NULL, 16);
  return INT;
				}
\"([^\"\\\r\n]|\\[\"\\bfnrtav])*\" {
  char *str;
  size_t len;
  FILE *buf = open_memstream (&str, &len);
  size_t i;
  for (i = 1; yytext[i] != '"'; ++i)
    {
      if (yytext[i] == '\\')
	switch (yytext[++i])
	  {
	  case '\'':
	  case '\\':
	    putc (yytext[i], buf);
	    break;
	  case 'b':
	    putc ('\b', buf);
	    break;
	  case 'f':
	    putc ('\f', buf);
	    break;
	  case 'n':
	    putc ('\n', buf);
	    break;
	  case 'r':
	    putc ('\r', buf);
	    break;
	  case 't':
	    putc ('\t', buf);
	    break;
	  case 'a':
	    putc ('\a', buf);
	    break;
	  case 'v':
	    putc ('\v', buf);
	    break;
	  default:
	    break;
	  }
      else
        putc (yytext[i], buf);
    }
  fflush (buf);
  fclose (buf);
  yylval.sval = str;
  return STR;
}
\'([^\'\\\r\n]|\\[\'\\bfnrtav])\' {
  if (yytext[1] == '\\')
    switch (yytext[2])
      {
      case '\'':
      case '\\':
	yylval.ival = yytext[2];
	break;
      case 'b':
	yylval.ival = '\b';
	break;
      case 'f':
	yylval.ival = '\f';
	break;
      case 'n':
	yylval.ival = '\n';
	break;	
      case 'r':
	yylval.ival = '\r';
	break;
      case 't':
	yylval.ival = '\t';
	break;
      case 'a':
	yylval.ival = '\a';
	break;
      case 'v':
	yylval.ival = '\v';
	break;
      default:
	break;
      }
  else
    yylval.ival = yytext[1];
  return INT;
				}
\.(GLOBAL)|(global)		return D_GLOBAL;
\.(SECTION)|(section)		return D_SECTION;
\.(STACK)|(stack)		return D_STACK;	
\.(ESTACK)|(estack)		return D_ESTACK;
TEXT|text			return S_TEXT;
DATA|data			return S_DATA;
STDOUT|stdout			return S_STDOUT;
STDERR|stderr			return S_STDERR;
STDIN|stdin			return S_STDIN;
HALT|halt			return K_HALT;
MOV|mov				return K_MOV;
MH32|mh32			return K_MH32;
ML32|ml32			return K_ML32;
ML16|ml16			return K_ML16;
ML8|ml8				return K_ML8;
SWP|swp				return K_SWP;
I2F|i2f				return K_I2F;
B2F|b2f				return K_B2F;
F2I(F)?|f2i(f)?			return K_F2IF;
F2B|f2b				return K_F2B;
F2IC|f2ic			return K_F2IC;
RMEH|rmeh			return K_RMEH;
THROW|throw			return K_THROW;
PUSH|push			return K_PUSH;
POP|pop				return K_POP;
LDEX|ldex			return K_LDEX;
PLDEX|pldex			return K_PLDEX;
ADD|add				return K_ADD;
SUB|sub				return K_SUB;
MUL|mul				return K_MUL;
DIV|div				return K_DIV;
MOD|mod				return K_MOD;
AND|and				return K_AND;
OR|or				return K_OR;
XOR|xor				return K_XOR;
NOT|not				return K_NOT;
LSH|lsh				return K_LSH;
RSH|rsh				return K_RSH;
SRSH|srsh			return K_SRSH;
ROL|rol				return K_ROL;
ROR|ror				return K_ROR;
CALL|call			return K_CALL;
JMP|jmp				return K_JMP;
RET|ret				return K_RET;
JE|je				return K_JE;
JLS|jls				return K_JLS;
JGS|jgs				return K_JGS;
JL|jl				return K_JL;
JG|jg				return K_JG;
JOF|jof				return K_JOF;
JZ|jz				return K_JZ;
INEH|ineh			return K_INEH;
LDC|ldc				return K_LDC;
LDB|ldb				return K_LDB;
LDW|ldw				return K_LDW;
LDD|ldd				return K_LDD;
LDQ|ldq				return K_LDQ;
STB|stb				return K_STB;
STW|stw				return K_STW;
STD|std				return K_STD;
STQ|stq				return K_STQ;
LD(S)?|ld(s)?			return K_LDS;
ST(S)?|st(s)?			return K_STS;
STFB(S)?|stfb(s)?		return K_STFBS;
ALLOC|alloc			return K_ALLOC;
FREE|free			return K_FREE;
SJE|sje				return K_SJE;
SJL|sjl				return K_SJL;
SJSL|sjsl			return K_SJSL;
SJG|sjg				return K_SJG;
SJSG|sjsg			return K_SJSG;
SJZ|sjz				return K_SJZ;
FOPEN|fopen			return K_FOPEN;
FCLOSE|fclose			return K_FCLOSE;
FFLUSH|fflush			return K_FFLUSH;
FREWIND|frewind			return K_FREWIND;
FREAD|fread			return K_FREAD;
FWRTB|fwrtb			return K_FWRTB;
FWRTQ|fwrtq			return K_FWRTQ;
FWRTS|fwrts			return K_FWRTS;
DB|db				return S_DB;
DW|dw				return S_DW;
DD|dd				return S_DD;
DQ|dq				return S_DQ;
-?(0|[1-9][0-9]*)	       	{
  yylval.ival = strtol (yytext, NULL, 10);
  return INT;
				}
[$_a-zA-Z][$_a-zA-Z0-9]*	{
  yylval.sval = strdup (yytext);
  return LABEL;
				}
#[^\n\r]*			;
.				yyerror ("Wtf!?");

%%
