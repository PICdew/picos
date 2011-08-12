%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <getopt.h>
#include <errno.h>
#include "pasm.h"
#include "../piclang.h"
#include "utils.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

struct compiled_code *the_code;
struct compiled_code *the_code_end;
struct compiled_code *the_strings;
struct compiled_code *the_strings_end;
char **string_list;
size_t num_strings;
int *variable_list;
size_t num_variables;
static int lbl;

#define insert_string(X) insert_compiled_code(&the_strings,&the_strings_end,X)
#define insert_code(X) insert_compiled_code(&the_code,&the_code_end,X)

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(int value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);
 FILE *assembly_file;
void yyerror(char *s);
 int resolve_string(const char *str, int *is_new);
 int resolve_variable(const int id);
 extern char *yytext;
 extern char *last_string;
 nodeType* store_string(const char *);
int sym[26];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER 
%token <sIndex> VARIABLE
%token WHILE IF PUTCH PUTD EXIT INPUT SYSTEM SPRINT STRING CR
%token MORSE TIME
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list STRING

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
        | SYSTEM '(' expr ',' expr ')' ';'{ $$ = opr(SYSTEM,2,$3,$5);}
        | SYSTEM '(' expr ',' expr ',' expr ')' ';' {$$ = opr(SYSTEM,3,$3,$5,$7);}
        | SPRINT '(' STRING ')' ';' {$$ = opr(SPRINT,1,$3);}
        | MORSE '(' STRING ')' ';' {$$ = opr(MORSE,1,$3);}
        | TIME '(' ')' ';'{$$ = opr(TIME,0);}
        | INPUT VARIABLE ';'             { $$ = opr(INPUT, 1, id($2)); }
        | expr ';'                       { $$ = $1; }
        | PUTCH '(' expr ')' ';'                 { $$ = opr(PUTCH, 1, $3); }
        | PUTD '(' expr ')' ';'        { $$ = opr(PUTD,1,$3); }
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





static const char short_options[] = "a:b:e:ho:";
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
               {"hex", 1,NULL, 'o'},
	       {"asm", 1,NULL, 'a'},
	       {"eeprom",1,NULL, 'e'},
	       {"binary",1,NULL,'b'},
               {0, 0, 0, 0}
             };

void print_help()
{
  printf("\n");
  printf("pasm -- Piclang compiler.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Compiles piclang programs for use with the Pic Operating System.\n");
  printf("Note: If no source file is provided, the compiler will act as \n\tan interpreter.\n");
  printf("\n");
  printf("Usage: pasm [options] [source code]\n\n");
  printf("Options:\n");
  printf("--help, -h :\t\t Displays this dialog.\n");
  printf("--asm,-a <file> :\t Outputs the assembly to the specified file.\n");
  printf("--hex,-o <file> :\t Outputs Intel Hex to the specified file.\n");
  printf("--eeprom, -e <file> :\t Outputs \"__EEPROM_DATA(...)\" code for use\n");
  printf("                     \t with the Hi Tech C Compiler.\n");
  printf("--binary, -b <file> :\t Outputs a binary file containing the compiled program.\n");
}

int main(int argc, char **argv) 
{
  char hex_buffer[45];
  FILE *hex_file = stdout, *eeprom_file = stdout, *binary_file = NULL;
  char opt;
  int opt_index;
  unsigned char piclang_bitmap = 0;
  struct compiled_code *curr_code = NULL;

  assembly_file = NULL;
  the_code_end = the_code = NULL;
  the_strings = the_strings = NULL;
  string_list = NULL;num_strings = 0;
  variable_list = NULL;num_variables = 0;

  while(TRUE)
    {    
      opt = getopt_long(argc,argv,short_options,long_options,&opt_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case 'o':
	  hex_file = fopen(optarg,"w");
	  if(hex_file == NULL)
	    hex_file = stdout;
	  break;
	case 'a':
	  assembly_file = fopen(optarg,"w");
	  if(assembly_file == NULL)
	    assembly_file = stdout;
	  break;
	case 'b':
	  binary_file = fopen(optarg,"w");
	  if(binary_file == NULL)
	    {
	      fprintf(stderr,"Could not open %s for writing.\n",optarg);
	      exit(ENOENT);
	    }
	  break;
	case 'e':
	  eeprom_file = fopen(optarg,"w");
	  if(eeprom_file == NULL)
	    eeprom_file = stdout;
	  break;
	case 'h':
	  print_help();
	  return 0;
	default:
	  fprintf(stderr,"ERROR - Unknown flag %c\n",opt);
	  print_help();
	  return -1;
	}
    }
  printf("Welcome to the piclang compiler.\n");

  if(optind < argc)
    {
      FILE *input = fopen(argv[optind++],"r");
      extern FILE *yyin;
      if(input != NULL)
	yyin = input;
      if(assembly_file == NULL)
	{
	  printf("Assembly:\n");
	  assembly_file = stdout;
	}
    }

  
  yyparse();
  insert_compiled_code(&the_code,&the_code_end,EOP);

  if(hex_file == stdout)
    printf("Here comes your code.\nThank you come again.\nCODE:\n");
  pasm_compile(eeprom_file,hex_file,&the_code,the_strings,&piclang_bitmap,num_variables);

  if(binary_file != NULL)
    {
      curr_code = the_code;
      while(curr_code != NULL)
	{
	  fputc(curr_code->val,binary_file);
	  curr_code = curr_code->next;
	}
    }
  
  FreeCode(the_code);
  return 0;
}

int ex(nodeType *p) {
    int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
      write_assembly(assembly_file,"\tpushl\t%d\n", p->con.value); 
      insert_code(PICLANG_PUSHL);
      insert_code(p->con.value);
      break;
    case typeId:        
      write_assembly(assembly_file,"\tpush\t%c\n", p->id.i + 'a');
      insert_code(PICLANG_PUSH);
      insert_code(resolve_variable(p->id.i));
      break;
    case typeStr:
      {
	char *pStr = p->str.string;
	if(pStr != NULL)
	  {
	    nodeType str_pointer;
	    int is_new = TRUE;
	    str_pointer.con.value = resolve_string(pStr,&is_new);
	    str_pointer.type = typeCon;
	    if(is_new)
	      {
		write_assembly(assembly_file,"\tstore\t\"%s\"\n", pStr);
		while(pStr != NULL)
		  {
		    insert_string(*pStr);
		    if(*pStr == 0)
		      break;
		    pStr++;
		  }
	      }
	    ex(&str_pointer);
	  }
	break;
      }
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            write_assembly(assembly_file,"L%03d:\n", lbl1 = lbl++);
            ex(p->opr.op[0]);
            write_assembly(assembly_file,"\tjz\tL%03d\n", lbl2 = lbl++);
            ex(p->opr.op[1]);
            write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl1);
            write_assembly(assembly_file,"L%03d:\n", lbl2);
            break;
        case IF:
            ex(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                write_assembly(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl2 = lbl++);
                write_assembly(assembly_file,"L%03d:\n", lbl1);
                ex(p->opr.op[2]);
                write_assembly(assembly_file,"L%03d:\n", lbl2);
            } else {
                /* if */
                write_assembly(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                write_assembly(assembly_file,"L%03d:\n", lbl1);
            }
            break;
        case PUTD:     
	  ex(p->opr.op[0]);
           write_assembly(assembly_file,"\tputd\n");insert_code(PICLANG_PRINTL);
            break;
        case PUTCH:
            ex(p->opr.op[0]);
            write_assembly(assembly_file,"\tputch\n");insert_code(PICLANG_PRINT);
            break;
        case '=':       
            ex(p->opr.op[1]);
            write_assembly(assembly_file,"\tpop\t%c\n", p->opr.op[0]->id.i + 'a');insert_code( PICLANG_POP);insert_code(resolve_variable(p->opr.op[0]->id.i));
            break;
        case UMINUS:    
            ex(p->opr.op[0]);
            write_assembly(assembly_file,"\tneg\n");
            break;
	case INPUT:
	  //ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tpushl\t%c\n",p->opr.op[0]->id.i + 'a');
	  insert_code(PICLANG_PUSHL);
	  insert_code(resolve_variable(p->opr.op[0]->id.i));
	  write_assembly(assembly_file,"\tinput\n");
	  insert_code( PICLANG_INPUT);
	  break;
        case SYSTEM:
	  {
	    int op_counter = p->opr.nops - 1;
	    for(;op_counter >= 0 ;op_counter--)
		ex(p->opr.op[op_counter]);
	    write_assembly(assembly_file,"\tsystem\n");insert_code(PICLANG_SYSTEM);
	    break;
	  }
	case SPRINT:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tsprint\n");
	  insert_code(PICLANG_SPRINT);
	  break;
	case MORSE:
	  ex(p->opr.op[0]);
	  write_assembly(assembly_file,"\tmorse\n");
	  insert_code(PICLANG_MORSE);
	  break;
	case TIME:
	  write_assembly(assembly_file,"\ttime\n");
	  insert_code(PICLANG_TIME);
	  break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   write_assembly(assembly_file,"\tadd \n"); insert_code(PICLANG_ADD);break;
            case '-':   write_assembly(assembly_file,"\tsub\n");insert_code(PICLANG_SUB); break; 
            case '*':   write_assembly(assembly_file,"\tmul\n");insert_code(PICLANG_MULT); break;
            case '/':   write_assembly(assembly_file,"\tdiv\n"); break;
            case '<':   write_assembly(assembly_file,"\tcompLT\n"); break;
            case '>':   write_assembly(assembly_file,"\tcompGT\n"); break;
            case GE:    write_assembly(assembly_file,"\tcompGE\n"); break;
            case LE:    write_assembly(assembly_file,"\tcompLE\n"); break;
            case NE:    write_assembly(assembly_file,"\tcompNE\n"); break;
            case EQ:    write_assembly(assembly_file,"\tcompEQ\n"); break;
            }
        }
    }
    return 0;
}

int resolve_string(const char *str, int *is_new)
{
  size_t i,retval;
  if(string_list == NULL)
    {
      string_list = (char**)malloc(sizeof(char*));
      string_list[0] = strdup(str);
      num_strings = 1;
      if(is_new != NULL)
	*is_new = TRUE;
      return 0;
    }
  
  if(is_new != NULL)
    *is_new = FALSE;
  i = 0;
  retval = 0;
  for(;i<num_strings;i++)
    if(strcmp(string_list[i],str) == 0)
      return retval;
    else
      retval += strlen(string_list[i]) + 1;
  
  if(is_new != NULL)
    *is_new = TRUE;
  string_list = (char**)realloc(string_list,(num_strings+1)*sizeof(char*));
  string_list[num_strings++] = strdup(str);
  return retval;
}

int resolve_variable(const int id)
{
  int i;
  if(variable_list == NULL)
    {
      variable_list = (int*)malloc(sizeof(int));
      variable_list[0] = id;
      num_variables = 1;
      return 0;
    }
  
  i = 0;
  for(;i<num_variables;i++)
    if(variable_list[i] == id)
      return i;
  
  variable_list = (int*)realloc(variable_list,(num_variables+1)*sizeof(int));
  variable_list[num_variables++] = id;
  return num_variables - 1;
  
}






