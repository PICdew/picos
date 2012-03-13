%{
#include "pasm.h"
#include "../piclang.h"
#include "utils.h"
#include "../page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>
#include <getopt.h>
#include <errno.h>

struct compiled_code *the_code;
struct compiled_code *the_code_end;
struct compiled_code *the_strings;
struct compiled_code *the_strings_end;
struct subroutine_map *subroutines;

extern struct assembly_map opcodes[];

char **string_list;
size_t num_strings;
picos_size_t FS_BUFFER_SIZE;

idNodeType *variable_list = NULL;// Variable table
extern picos_size_t label_counter;
 int break_to_label, continue_to_label;

/* prototypes */
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);
 FILE *assembly_file;
 FILE *lst_file;
void yyerror(char *s);
 int resolve_string(const char *str, int *is_new);
 int resolve_variable(const char *name);// Looks up a variable and retrieves its page memory index. If the variable does not yet exist, it will be added to the list.
 extern char *yytext;
 extern char *last_string;
 nodeType* store_string(const char *);
  
%}

%union {
    int iValue;                 /* integer value */
    idNodeType variable;          /* symbol table index */
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER FUNCT
%token <variable> VARIABLE
%type <nPtr> stmt expr STRING SUBROUTINE

%token WHILE BREAK CONTINUE IF CALL SUBROUTINE
%token STRING RETURN DEFINE EXIT 
%token PASM_CR PASM_POP ARGV ARGC FIN FEOF STATEMENT_DELIM

%nonassoc IFX
%nonassoc ELSE

%left BSL BSR GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%left '%'
%nonassoc UMINUS

%locations

%%

program:
        function                { YYACCEPT; }
        ;

function: 
          function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt: 
'\n'                            { $$ = opr(PASM_STATEMENT_DELIM, 2, NULL, NULL); }
        | RETURN stmt                     { $$ = opr(PICLANG_RETURN,1,$2);}
        | CALL SUBROUTINE      { $$ = opr(PICLANG_CALL,1,$2); }
        | SUBROUTINE stmt       {  $$ = opr(PASM_LABEL,2,$1,$2);}
        | FUNCT SUBROUTINE      {$$ = opr($1,1,$2); }
        | PASM_CR                 { $$ = opr(PICLANG_PRINTL,1,con(0xa));}
        | VARIABLE '=' expr       { $$ = opr(PICLANG_POP, 2, id($1), $3); }
| VARIABLE '=' STRING       { $$ = opr(PICLANG_POP, 2, id($1), con(handle_string($3->str.string))); }
        | PASM_POP VARIABLE      { $$ = opr(PICLANG_POP,1,id($2)); }
        | STRING                 { handle_string($1->str.string); }
        | expr '\n' { $$ = $1; }
        | EXIT {YYACCEPT;}
        ;

expr:
          INTEGER               { $$ = con($1); }
        | VARIABLE              { $$ = id($1); }
        | ARGC                  { $$ = opr(PICLANG_ARGC,0); }
        | FIN                   { $$ = con(ARG_SIZE); }
        | FEOF                   { $$ = con(((picos_size_t)(-1))); }
        | ARGV '[' expr ']'     { $$ = opr(PICLANG_ARGV,1,$3); }
        | FUNCT expr    { $$ = opr($1,1,$2); }
        | FUNCT         { $$ = opr($1,0); }
        ;

%%

static const char short_options[] = "a:b:e:hl:o:";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
               {"hex", 1,NULL, OUTPUT_HEX},
	       {"asm", 1,NULL, 'a'},
	       {"eeprom",1,NULL, 'e'},
	       {"binary",1,NULL,'o'},
	       {"list",1,NULL,'l'},
	       {"buffer_size",1,NULL,'b'},
               {0, 0, 0, 0}
             };

void print_help()
{
  printf("\n");
  printf("pasm -- Piclang assembler.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Compiles piclang programs for use with the Pic Operating System.\n");
  printf("Note: If no source file is provided, the compiler will act as \n\tan interpreter.\n");
  printf("\n");
  printf("Usage: pasm [options] [source code]\n\n");
  printf("Options:\n");
  printf("--help, -h :\t\t Displays this dialog.\n");
  printf("--asm,-a <file> :\t Outputs the assembly to the specified file.\n");
  printf("--hex <file>    :\t Outputs Intel Hex to the specified file.\n");
  printf("--eeprom, -e <file> :\t Outputs \"__EEPROM_DATA(...)\" code for use\n");
  printf("                     \t with the Hi Tech C Compiler.\n");
  printf("--binary, -o <file> :\t Outputs a binary file containing the compiled program.\n");
  printf("--list, -l <file> :\t Outputs a list of program addresses (PC values) for each assembly entry.\n");
  printf("--block_size, -b <INT> :\t Sets the size of block of the target PICFS (Default: 128)");
}

int main(int argc, char **argv) 
{
  char hex_buffer[45];
  FILE *hex_file = NULL, *eeprom_file = NULL, *binary_file = NULL;
  char opt;
  int opt_index;
  picos_size_t piclang_bitmap = 0;
  struct compiled_code *curr_code = NULL;

  assembly_file = NULL;
  the_code_end = the_code = NULL;
  the_strings = the_strings = NULL;
  string_list = NULL;num_strings = 0;
  variable_list = NULL;
  subroutines = NULL;
  break_to_label = -1;
  continue_to_label = -1;
  FS_BUFFER_SIZE = 128;
  
  while(true)
    {    
      opt = getopt_long(argc,argv,short_options,long_options,&opt_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case OUTPUT_HEX:
	  hex_file = fopen(optarg,"w");
	  if(hex_file == NULL)
	    hex_file = stdout;
	  break;
	case 'b':
	  if(sscanf(optarg,"%hu",&FS_BUFFER_SIZE) != 1)
	    {
	      fprintf(stderr,"Could not read buffers size: %s\n",optarg);
	      exit(-1);
	    }
	  break;
	case 'a':
	  assembly_file = fopen(optarg,"w");
	  if(assembly_file == NULL)
	    assembly_file = stdout;
	  break;
	case 'l':
	  lst_file = fopen(optarg,"w");
	  if(lst_file == NULL)
	    lst_file = stdout;
	  break;
	case 'o':
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

  if(optind < argc)
    {
      FILE *input = fopen(argv[optind++],"r");
      extern FILE *yyin;
      if(input != NULL)
	yyin = input;
    }
  else
    printf("Welcome to the piclang assembler.\n");
  
  yyparse();
  insert_code(PICLANG_PUSHL);
  insert_code(PICLANG_SUCCESS);
  insert_code(PICLANG_EXIT);

  if(hex_file == stdout)
    printf("Here comes your code.\nThank you come again.\nCODE:\n");
  pasm_compile(eeprom_file,hex_file,&the_code,the_strings,&piclang_bitmap,count_variables());

  if(binary_file != NULL)
    {
      curr_code = the_code;
      while(curr_code != NULL)
	{
	  if(curr_code->type != typeStr)
	    write_val_for_pic(binary_file,curr_code->val);
	  else
	    fprintf(binary_file,"%c",(char)curr_code->val);
	  curr_code = curr_code->next;
	}
    }

  if(lst_file != NULL)
    {
      struct assembly_map* curr;
      struct compiled_code *first_string = NULL;
      int code_counter = 0;
      curr_code = the_code;
      for(;curr_code != NULL;curr_code = curr_code->next)
	{
	  if(curr_code->type == typePCB)
	    continue;
	  if(curr_code->type == typeStr)
	    {
	      continue;
	    }
	  curr = opcode2assembly(curr_code->val);
	  fprintf(lst_file,"(%d)\t",code_counter++);
	  switch(curr->opcode)
	    {
	    case PICLANG_NUM_COMMANDS:
	      if(curr_code->type == typeLabel)
		{
		  fprintf(lst_file,"L%03hu",curr_code->label);
		  break;
		}
	      else if(curr_code->val == PICLANG_RETURN)
		{
		  fprintf(lst_file,"return");
		  break;
		}
	      else if(curr_code->val == PICLANG_CALL)
		{
		  curr_code = curr_code->next;
		  while(curr_code->next != NULL && curr_code->type == typeStr)
		    curr_code = curr_code->next;
		  fprintf(lst_file,"call %hu",curr_code->val);
		  code_counter++;
		  break;
		}
	    default:
	      fprintf(lst_file,"%s (0x%x) ",curr->keyword,curr_code->val);
	      break;
	    }
	  if(curr->has_arg)
	    {
	      curr_code = curr_code->next;
	      code_counter++;
	      fprintf(lst_file," %d",curr_code->val);
	      if(curr->has_arg > 1)
		{
		  int arg_counter = 1;
		  for(;arg_counter < curr->has_arg;arg_counter++)
		    {
		      code_counter++;
		      curr_code = curr_code->next;
		      fprintf(lst_file,", %d",curr_code->val);
		    }
		}
	    }
	  fprintf(lst_file,"\n");
	}
      // print strings
      first_string = the_strings;
      if(first_string != NULL)
	fprintf(lst_file,"Strings:\n\"");
      for(;first_string != NULL;first_string = first_string->next)
	{
	  if(first_string->val == 0)
	    {
	      fprintf(lst_file,"\"\n");
	      if(first_string->next != NULL)
		fprintf(lst_file,"\"");
	    }
	  else if(first_string->val == '"')
	    fprintf(lst_file,"\"%c",first_string->val);
	  else
	    fprintf(lst_file,"%c",first_string->val);
	}
    }
  
  FreeCode(the_code);
  return 0;
}
