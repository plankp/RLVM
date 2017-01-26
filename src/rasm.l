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
-?0|[1-9][0-9]*			{
  yylval.ival = atoi (yytext);
  return INT;
				}
[$_a-zA-Z][$_a-zA-Z0-9]*	{
  yylval.sval = strdup (yytext);
  return LABEL;
				}
#[^\n\r]*			;
.				yyerror ("Wtf!?");

%%
