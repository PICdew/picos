%{
#include <stdlib.h>
#include "pasm.h"
#include "pasm_yacc.h"
void yyerror(char *);

unsigned char do_crc(const char *str)
{
  char crc;
  if(str == NULL)
    return 0;

  crc = 0;
  while(*str != NULL)
    {
      calculate_crc(&crc,*str);
      str++;
    }
  return crc;
}

%}


%s C_COMMENT

%%

"/*"            BEGIN(C_COMMENT);
<C_COMMENT>"*/" {BEGIN(INITIAL);}
<C_COMMENT>. {}

\"[a-zA-Z][a-zA-Z0-9]*\" {
                              yylval.iValue = do_crc(yytext);
                              return STRING;
                         }

[a-z]       { 
                yylval.sIndex = *yytext - 'a';
                return VARIABLE;
            }

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;{}.] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"input"         return INPUT;
"system"        return SYSTEM;
"quit"          return EXIT;

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
