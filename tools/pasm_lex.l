%{
#include "pasm.h"
#include "pasm_yacc.h"
#include "piclang.h"
void yyerror(char *);

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>


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

%x C_COMMENT

whitespace [ \t\n]+
zero 0
nonnegative [0-9]
integer [1-9]{nonnegative}*
exponent [eE][+-]{nonnegative}*
decimal {zero}?{integer}*\.{nonnegative}*{exponent}?
alpha [a-zA-Z]
variable {alpha}({nonnegative}|{alpha}|[_])*
hex 0x[0-9a-fA-F]*
octal 0[0-9a-fA-F]*
imagint {integer}[iI]
imagdecimal {decimal}[iI]


%%

"/*"            BEGIN(C_COMMENT);
<C_COMMENT>"*/" {BEGIN(INITIAL);}
<C_COMMENT>. {}


">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"call"          return CALL;
"return"        return RETURN;
"def"           return DEFINE;
"quit"          return EXIT;
"cr"            return PASM_CR;
"pop"           return PASM_POP;


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

\<{variable}\>  {
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

{variable} {
              struct assembly_map *asmb = keyword2assembly(yytext);
              if(asmb == NULL || asmb->opcode == PICLANG_NUM_COMMANDS)
                 yyerror("syntax error");
              yylval.iValue = asmb->opcode;
              return FUNCT;
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

[ \t\n]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
