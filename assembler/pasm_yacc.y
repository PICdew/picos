%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "pasm.h"
#include "../piclang.h"
#include "page.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);

void yyerror(char *s);
int sym[26];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <sIndex> VARIABLE
%token WHILE IF PRINT PRINTL EXIT INPUT
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
        function                { YYACCEPT; }
        ;

function: 
          function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt: 
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | INPUT VARIABLE ';'             { $$ = opr(INPUT, 1, id($2)); }
        | expr ';'                       { $$ = $1; }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'              { $$ = $2; }
        | EXIT {YYACCEPT;}
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | VARIABLE              { $$ = id($1); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id(int i) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}


#define COMPILE_MAX_WIDTH 8//max width

void FirstPass(struct compiled_code* code, int *variable_map, int skip_assignment_check,   int *next_memory_slot)
{
  int clean_skip_assignment_check = skip_assignment_check;
  
  if(code == NULL)
    return;
    switch(code->val)
    {
    case PICLANG_PUSH:case PICLANG_POP:case PICLANG_INPUT:case PICLANG_PRINT:
      if(!skip_assignment_check && code->next != NULL && (code->next->val <= 'z' - 'a'))
	{
	  if(variable_map[(size_t)code->next->val] == -1)
	    variable_map[(size_t)code->next->val] = *(next_memory_slot++);
	  code->next->val = variable_map[(size_t)code->next->val];
	  skip_assignment_check = TRUE;
	}
    }
  if(clean_skip_assignment_check)
    skip_assignment_check = FALSE;
 FirstPass(code->next,variable_map,skip_assignment_check,next_memory_slot);
}

void PrintCode(struct compiled_code* code, int col)
{
  if(code == NULL)
    return;
  if(col == 0)
    printf(":");

 printf("%02x",code->val);
  col++;
  if(col >= COMPILE_MAX_WIDTH)
    {
      col = 0;
      printf("\n");
    }
  
  PrintCode(code->next,col);
}

void FreeCode(struct compiled_code* code)
{
  if(code == NULL)
    return;
  FreeCode(code->next);
  free(code);
}

size_t CountCode(struct compiled_code *the_code)
{
  if(the_code == NULL)
    return 0;
  return 1 + CountCode(the_code->next);
}

struct compiled_code* MakePCB(struct compiled_code *the_code, int total_memory)
{
  struct compiled_code *size = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  size->val = CountCode(the_code);
  struct compiled_code *num_pages = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  num_pages->val = (unsigned char)ceil(total_memory/PAGE_SIZE);
  struct compiled_code *pc = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  pc->val = 0;
  struct compiled_code *status = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  status->val = PICLANG_SUCCESS;
  struct compiled_code *start_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  start_address->val = 0;
  struct compiled_code *stack = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *end_of_stack = stack;
  size_t stack_counter = 1;
  for(;stack_counter < PICLANG_STACK_SIZE;stack_counter++)
    {
      end_of_stack->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      end_of_stack = end_of_stack->next;
    }
  end_of_stack->next  = (struct compiled_code*)malloc(sizeof(struct compiled_code));// this is the stack head pointer.
  end_of_stack->next->val = 0;
  size->next = num_pages;
  num_pages->next = pc;
  pc->next = status;
  status->next = start_address;
  start_address->next = stack;
  
  end_of_stack->next = the_code;
  return size;
}


int main(int argc, char **argv) 
{
  the_code_end = the_code = NULL;
  printf("Welcome to the piclang compiler.\n");
  if(argc > 1)
    {
      FILE *input = fopen(argv[1],"r");
      extern FILE *yyin;
      if(input != NULL)
	yyin = input;
    }
  yyparse();
  printf("Here comes your code.\nThank you come again.\nCODE:\n");
  insert_code(EOP);
  int variable_map['z'-'a'+1];
  int total_memory = 0;
  memset(variable_map,-1,('z'-'a'+1)*sizeof(int));
  FirstPass(the_code,variable_map,FALSE,&total_memory);
  the_code = MakePCB(the_code,total_memory);
  PrintCode(the_code,0);
  printf("\n");
  FreeCode(the_code);
  return 0;
}
