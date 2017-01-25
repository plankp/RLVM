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
  yylval.ival = atoi (yytext + 1);
  return FREG;
				}
[rR]([0-9]|[12][0-9]|3[01])	{
  yylval.ival = atoi (yytext + 1);
  return IREG;
				}
HALT|halt			return K_HALT;
0|[1-9][0-9]*			{
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
