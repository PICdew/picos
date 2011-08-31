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

\'[^\']\' {
                              yylval.iValue = yytext[1];
                              return INTEGER;
                         }

\"[^\"]*\" {
                              size_t text_len = strlen(&yytext[1]);
                              yylval.nPtr = (nodeType*)malloc(sizeof(nodeType));
                              strncpy(yylval.nPtr->str.string,&yytext[1],text_len-1);// minus 1 for the end quotation mark
                              yylval.nPtr->str.string[text_len-1] = 0;
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
                 sscanf(yytext,"0%o",&yylval.iValue);
                  return INTEGER;
                 }

\<[a-zA-Z_][0-9a-zA-Z_]*\>  {
                size_t text_len = strlen(&yytext[1]);
                yylval.nPtr = (nodeType*)malloc(sizeof(nodeType));
                strncpy(yylval.nPtr->str.string,&yytext[1],text_len-1);// minus 1 for the end quotation mark
                yylval.nPtr->str.string[text_len-1] = 0;
                yylval.nPtr->type = typeSubroutine;
                return SUBROUTINE;
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
"getch"         return GETCH;
"getd"          return GETD;
"fputch"        return FPUTCH;
"fputd"        return FPUTD;
"fflush"        return FFLUSH;
"system"        return SYSTEM;
"sprint"        return SPRINT;
"morse"         return MORSE;
"settime"       return SET_TIME;
"setdate"       return SET_DATE;
"time"          return TIME;
"argd"          return ARGD;
"argch"         return ARGCH;
"cr"            return CR;
"clear"         return CLEAR;
"call"          return CALL;
"exit"          return EXIT_RETURN;
"return"        return RETURN;
"def"           return DEFINE;
"end"           return ENDDEF;
"quit"          return EXIT;

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
