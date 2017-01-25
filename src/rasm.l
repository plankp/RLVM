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
