%{
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include "pasm.h"
#include "pasm_yacc.h"
void yyerror(char *);

unsigned char do_crc(const char *str)
{
  char crc;
  if(str == NULL)
    return 0;

  crc = 0;
  while(*str != 0)
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

\'[a-zA-Z][a-zA-Z0-9]*\' {
                              yylval.iValue = do_crc(yytext);
                              return INTEGER;
                         }

\"[a-zA-Z.][a-zA-Z0-9 ]*\" {
                              yylval.nPtr = (nodeType*)malloc(sizeof(nodeType));
                              strncpy(yylval.nPtr->str.string,&yytext[1],strlen(yytext)-2);
                              yylval.nPtr->type = typeStr;
                              return STRING;
                         }

\\. {
           yylval.iValue = (int)yytext[1];
           return INTEGER;
     }

0x[0-9a-fA-F][0-9a-fA-F]* { 
                 sscanf(yytext,"0x%x",&yylval.iValue);
                  return INTEGER;
                 }

0[0-9a-fA-F][0-9a-fA-F]* { 
                 sscanf(yytext,"0x%o",&yylval.iValue);
                  return INTEGER;
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

[-,()<>=+*/;{}\[\]] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"putd"         return PUTD;
"putch"         return PUTCH;
"input"         return INPUT;
"system"        return SYSTEM;
"sprint"        return SPRINT;
"morse"         return MORSE;
"settime"       return SET_TIME;
"setdate"       return SET_DATE;
"time"          return TIME;
"argd"          return ARGD;
"cr"            return CR;
"quit"          return EXIT;

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
