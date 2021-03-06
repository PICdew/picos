%{
/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 20 June 2012
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * picosc lexicon
 */

#include "picos/tools/pasm.h"
#include "picos/tools/picosc_yacc.h"
#include "picos/piclang.h"
void yyerror(char *);

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#define YY_USER_ACTION {yylloc.first_line = yylineno; \
        yylloc.last_line = yylineno;}

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

%option yylineno

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

">>"            return BSR;
"<<"            return BSL;
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"continue"      return CONTINUE;
"break"         return BREAK;
"if"            return IF;
"else"          return ELSE;
"switch"        return SWITCH;
"case"          return CASE;
"default"       return DEFAULT;
"call"          return CALL;
"return"        return RETURN;
"def"           return DEFINE;
"quit"          return EXIT;
"cr"            return PASM_CR;
"argv"          return ARGV;
"argc"          return ARGC;
"fin"           return FIN;
"feof"          return FEOF;
"errno"         return ERRNO;
"const"         return CONST;

"SIGINT"       { yylval.iValue = PICOS_SIGINT; return INTEGER;}

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

\#[a-zA-Z]* {
             size_t text_len = strlen(&yytext[1]);
             yylval.nPtr = (nodeType*)malloc(sizeof(nodeType));
             strncpy(yylval.nPtr->str.string,&yytext[1],text_len);// minus 1 for the end quotation mark
             yylval.nPtr->str.string[text_len] = 0;
             yylval.nPtr->type = typePreproc;
             return PREPROC_KEYWORD;
            }


0x[0-9a-fA-F][0-9a-fA-F]* { 
                 sscanf(yytext,"0x%x",&yylval.iValue);
                  return INTEGER;
                 }

0[0-9a-fA-F][0-9a-fA-F]* { 
                 sscanf(yytext,"0%o",&yylval.iValue);
                  return INTEGER;
                 }

\<{variable}\>  { return parse_subroutine(1,1); }
L{nonnegative} { return parse_subroutine(1,0); }
"main" { return parse_subroutine(0,0); }




{variable} {
              struct assembly_map *asmb = keyword2assembly(yytext);
              if(asmb == NULL || asmb->opcode == PICLANG_NUM_COMMANDS)
              {
                strcpy(yylval.variable.name,yytext);
                yylval.variable.i = -1;
                yylval.variable.next = NULL;
                return VARIABLE;
              }
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

[-,()<>=+*/;{}\[\]%~&|:] {
                return *yytext;
             }

{whitespace}        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}


int parse_subroutine(size_t left_offset, size_t right_offset)
{
  size_t text_len = strlen(&yytext[left_offset]);
  yylval.nPtr = (nodeType*)malloc(sizeof(nodeType));
  strncpy(yylval.nPtr->str.string,&yytext[left_offset],text_len-right_offset);// minus 1 for the end quotation mark
  yylval.nPtr->str.string[text_len-right_offset] = 0;
  yylval.nPtr->type = typeSubroutine;
  return SUBROUTINE;
}
