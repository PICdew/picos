#include "pasm.h"
#include "globals.h"
#include "picosc_yacc.h"

#include "page.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <math.h>

extern void yyerror(char*);
extern char *yytext;
extern YYLTYPE yylloc;
picos_size_t label_counter = 0;

idNodeType* resolve_variable(const char *name);

static void deal_with_arguments(oprNodeType *opr)
{
  size_t op_index = 0;
  if(opr == NULL)
    return;
  for(;op_index < opr->nops;op_index++)
    ex(opr->op[op_index]);

}

int ex(nodeType *p) {
  extern FILE *assembly_file;
  struct compiled_code *inserted_word = NULL; 
  int lbl1, lbl2;
  int previous_break_to_label = break_to_label;
  int previous_continue_to_label = continue_to_label;
  //int previous_switch_end_label = switch_end_label;
  
  if (!p) return 0;
  switch(p->type) {
  case typeCon:
    write_assembly(assembly_file,"\tpushl\t0x%x\n", p->con.value);
    insert_code(PICLANG_PUSHL);
    inserted_word = insert_code(p->con.value);
    inserted_word->relocation_type = p->con.relocation_type;
    break;
  case typeId:  
    {
      idNodeType *var = resolve_variable(p->id.name);
      picos_size_t id = (var == NULL)? -1 : var->i;
      write_assembly(assembly_file,"\tpush\t%s\n", p->id.name);
      insert_code(PICLANG_PUSH);
      insert_code(id);
      break;
    }
  case typeOpr:
    switch(p->opr.oper) {
    case PICLANG_EXIT:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\texit\n",0); 
      insert_code(PICLANG_EXIT);
      break;
    case PICLANG_PUSH: case PICLANG_PUSHL:
      deal_with_arguments(&p->opr);
      break;
    case PICLANG_CALL:
      {
	const struct subroutine_map *subroutine = get_subroutine(p->opr.op[0]->str.string);
	write_assembly(assembly_file,"\tcall\t<%s>\n",subroutine->name);
	insert_code(PICLANG_CALL);
	insert_label(PASM_SUBROUTINE,subroutine->address);
	break;
      }
    case PASM_LABEL: case PASM_DEFINE:// KEEP RETURN AFTER DEFINE
      {
	const char *subroutine_name = p->opr.op[0]->str.string;
	struct subroutine_map *subroutine = NULL;
	struct subroutine_map *globals = NULL;
	lbl1 = label_counter;	label_counter++;
	subroutine = insert_subroutine(subroutine_name);
	if(subroutine == NULL)
	  {
	    fprintf(stderr,"Could not declare subroutine: %s\n",subroutine_name);
	    exit(-1);
	  }
	g_curr_subroutine = subroutine;
	globals = get_subroutine("GLOBALS");
	if(globals != NULL)
	{
		subroutine->strings = globals->strings;
		globals->strings = NULL;
	}
	subroutine->address = lbl1;
	write_assembly(assembly_file,"%s:\n", subroutine->name);
	insert_label(PICLANG_LABEL,lbl1);
	deal_with_arguments(&p->opr);
	if(strcmp(subroutine->name,"main") == 0 && p->opr.oper != PASM_LABEL)
	  {
	    write_assembly(assembly_file,"\tpushl\t0x0\n",0); 
	    insert_code(PICLANG_PUSHL);
	    insert_code(0);
	    write_assembly(assembly_file,"\texit\n",0); 
	    insert_code(PICLANG_EXIT);
	    break;
	  }
	if(p->opr.oper == PASM_LABEL)
	  break;
      }
    case PICLANG_RETURN:// KEEP RETURN AFTER DEFINE
      if(p->opr.oper == PICLANG_RETURN && p->opr.nops)
	deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\treturn\n");
      insert_code(PICLANG_RETURN);
      break;
    case PASM_CONTINUE:
      if(continue_to_label < 0)
	{
	  yyerror("Not within a block in which to continue");
	  exit(1);
	}
      write_assembly(assembly_file,"\tjmp\tL%03d\n", continue_to_label);
      insert_code(PICLANG_JMP);
      insert_label(PASM_ADDR,continue_to_label);
      break;
    case PASM_BREAK:
      if(break_to_label < 0)
	{
	  yyerror("Not within a block from which to break");
	  exit(-1);
	}
      write_assembly(assembly_file,"\tjmp\tL%03d\n", break_to_label);
      insert_code(PICLANG_JMP);
      insert_label(PASM_ADDR,break_to_label);
      break;
    case PASM_WHILE:
      write_assembly(assembly_file,"L%03d:\n", (lbl1 = label_counter));
      continue_to_label = lbl1;
      insert_label(PICLANG_LABEL,lbl1);
      label_counter++;
      ex(p->opr.op[0]);
      write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl2 = label_counter));
      label_counter++;
      break_to_label = lbl2;
      insert_code(PICLANG_JZ);
      insert_label(PASM_ADDR,lbl2);
      ex(p->opr.op[1]);
      write_assembly(assembly_file,"\tjmp\tL%03d\n", lbl1);
      insert_code(PICLANG_JMP);
      insert_label(PASM_ADDR,lbl1);
      write_assembly(assembly_file,"L%03d:\n", lbl2);
      insert_label(PICLANG_LABEL,lbl2);
      break_to_label = previous_break_to_label;
      continue_to_label = previous_continue_to_label;
      break;
    case PASM_IF:
      ex(p->opr.op[0]);
      if (p->opr.nops > 2) {
	/* if else */
	write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl1 = label_counter));
	label_counter++;
	insert_code(PICLANG_JZ);
	insert_label(PASM_ADDR,lbl1);
	ex(p->opr.op[1]);
	write_assembly(assembly_file,"\tjmp\tL%03d\n", (lbl2 = label_counter));
	label_counter++;
	insert_code(PICLANG_JMP);
	insert_label(PASM_ADDR,lbl2);
	write_assembly(assembly_file,"L%03d:\n", lbl1);
	insert_label(PICLANG_LABEL,lbl1);
	ex(p->opr.op[2]);
	write_assembly(assembly_file,"L%03d:\n", lbl2);
	insert_label(PICLANG_LABEL,lbl2);
      } else {
	/* if */
	write_assembly(assembly_file,"\tjz\tL%03d\n", (lbl1 = label_counter));
	label_counter++;
	insert_code(PICLANG_JZ);
	insert_label(PASM_ADDR,lbl1);
	ex(p->opr.op[1]);
	write_assembly(assembly_file,"L%03d:\n", lbl1);
	insert_label(PICLANG_LABEL,lbl1);
      }
      break;
    case PASM_SWITCH:
      {
	idNodeType *var = NULL;
	break_to_label = label_counter++;
	ex(p->opr.op[0]);// evaluate expression
	var = resolve_variable("_switch_expr");
	if(var == NULL)
	  {
	    fprintf(stderr,"ex: Error creating variable for switch");
	    exit(1);
	  }
	write_assembly(assembly_file,"\tpop _switch_expr\n");// pop expression into variable
	insert_code(PICLANG_POP);
	insert_code(var->i);

	ex(p->opr.op[1]);// go through cases

	write_assembly(assembly_file,"L%03d:\n", break_to_label);// this is the end of the switch
	insert_label(PICLANG_LABEL,break_to_label);
	break_to_label = previous_break_to_label;
	break; // PASM_SWITCH
      }
    case PASM_CASE:
      {
	idNodeType *var = NULL;
	int next_case = label_counter++;
	ex(p->opr.op[0]);// evaluate expression
	var = resolve_variable("_switch_expr");
	if(var == NULL)
	  {
	    fprintf(stderr,"ex: Error creating variable for switch");
	    exit(1);
	  }
	write_assembly(assembly_file,"\tpush _switch_expr\n");
	insert_code(PICLANG_PUSH);
	insert_code(var->i);// push switch expression
	write_assembly(assembly_file,"\tcompeq\n");// compare
	insert_code(PICLANG_COMPEQ);
	write_assembly(assembly_file,"\tjz L%03d\n",next_case);
	insert_code(PICLANG_JZ);
	insert_label(PASM_ADDR,next_case);
	ex(p->opr.op[1]);// evaluate statement
		
	write_assembly(assembly_file,"L%03d:\n",next_case);// next case label
	insert_label(PICLANG_LABEL,next_case);
	break;
      }
    case PICLANG_JZ: case PICLANG_JMP:
      {
	const struct subroutine_map *subroutine = get_subroutine(p->opr.op[0]->str.string);
	if(p->opr.oper == PICLANG_JZ)
	  write_assembly(assembly_file,"\tjz ");
	else
	  write_assembly(assembly_file,"\tjmp ");
	write_assembly(assembly_file,"%s;%d\n",subroutine->name,subroutine->address);
	break;
      }
    case PICLANG_PRINT:     
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tputd\n");insert_code(PICLANG_PRINTL);
      break;
    case PICLANG_ARGV:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\targv\n");
      insert_code(PICLANG_ARGV);
      break;
    case PICLANG_ARGC:
      write_assembly(assembly_file,"\targc\n");
      insert_code(PICLANG_ARGC);
      break;
    case PICLANG_LSOF:
      write_assembly(assembly_file,"\tlsof\n");
      insert_code(PICLANG_LSOF);
      break;
    case PICLANG_LSMOUNT:
      write_assembly(assembly_file,"\tlsmount\n");
      insert_code(PICLANG_LSMOUNT);
      break;
    case PICLANG_ERRNO:
      write_assembly(assembly_file,"\terrno\n");
      insert_code(PICLANG_ERRNO);
      break;
    case PICLANG_PRINTL:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tputch\n");insert_code(PICLANG_PRINT);
      break;
    case PICLANG_FPUTCH:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfputch\n");insert_code(PICLANG_FPUTCH);
      break;
    case PICLANG_FPUTD:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfputd\n");insert_code(PICLANG_FPUTD);
      break;
    case PICLANG_FOPEN:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfopen\n");insert_code(PICLANG_FOPEN);
      break;
    case PICLANG_FCLOSE:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfclose\n");insert_code(PICLANG_FCLOSE);
      break;
    case PICLANG_FREAD:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfread\n");insert_code(PICLANG_FREAD);
      break;
    case PICLANG_FSTAT:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tfstat\n");insert_code(PICLANG_FSTAT);
      break;
    case PICLANG_READDIR:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\treaddir\n");insert_code(PICLANG_READDIR);
      break;
    case PICLANG_DROP:
      write_assembly(assembly_file,"\tdrop\n");
      insert_code(PICLANG_DROP);
      break;
    case PICLANG_SWAP:
      write_assembly(assembly_file,"\tswap\n");
      insert_code(PICLANG_SWAP);
      break;
    case PICLANG_POP: case PASM_INITIALIZATION:
      {
	idNodeType *var = NULL;
	if(p->opr.nops == 0)
	  break;
	var = resolve_variable(p->opr.op[0]->id.name);
	if(var == NULL)
	  {
	    fprintf(stderr,"Compiler error: Variable list uninitialized");
	    exit(1);
	  }
	if(p->opr.nops > 1)
	  ex(p->opr.op[1]);
	if(p->opr.oper == PASM_INITIALIZATION)
	  {
	    var->constant = p->opr.op[0]->id.constant;
	    var->type = p->opr.op[0]->id.type;
	  }
	else if(p->opr.oper != PASM_INITIALIZATION && var->constant)
	  {
	    fprintf(stdout,"error: assignment of read-only variable '%s'\n",p->opr.op[0]->id.name);
	    exit(1);
	  }
	write_assembly(assembly_file,"\tpop\t%s\n", p->opr.op[0]->id.name);
	insert_code(PICLANG_POP);
	insert_code(var->i);
	break;
      }
    case PICLANG_UMINUS:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tneg\n");
      insert_code(PICLANG_UMINUS);
      break;
    case PICLANG_SIGNAL:
      {
	const struct subroutine_map *subroutine = NULL;
	if(p->opr.nops != 2)
	  {
	    fprintf(stderr,"Invalid number of arguments to signal()\nNeeded 2, got %d\n",p->opr.nops);
	    yyerror("Syntax error");
	  }
	subroutine = get_subroutine(p->opr.op[1]->str.string);
	if(subroutine == NULL)
	  {
	    fprintf(stderr,"Invalid subroutine: %s\n",p->opr.op[1]->str.string);
	    yyerror("Syntax error");
	  }
	write_assembly(assembly_file,"\tsignal %d, <%s>\n", p->opr.op[0]->con.value, subroutine->name);
	insert_code(PICLANG_SIGNAL);
	insert_code(p->opr.op[0]->con.value);
	insert_label(PASM_SUBROUTINE,subroutine->address);
	break;
      }
    case PICLANG_SLEEP:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tsleep\n");
      insert_code(PICLANG_SLEEP);
      break;
    case PICLANG_SPRINT:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tsprint\n");
      insert_code(PICLANG_SPRINT);
      break;
    case PICLANG_SPRINTN:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tsprintn\n");
      insert_code(PICLANG_SPRINTN);
      break;
    case PICLANG_MOVE:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tmove\n");
      insert_code(PICLANG_MOVE);
      break;
    case PICLANG_GETY:
      write_assembly(assembly_file,"\tgety\n");
      insert_code(PICLANG_GETY);
      break;
    case PICLANG_GETX:
      write_assembly(assembly_file,"\tgetx\n");
      insert_code(PICLANG_GETX);
      break;
    case PICLANG_RAWLOAD:
      write_assembly(assembly_file,"\trawload\n");
      insert_code(PICLANG_RAWLOAD);
      break;
    case PICLANG_MORSE:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tmorse\n");
      insert_code(PICLANG_MORSE);
      break;
    case PICLANG_TIME:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\ttime\n");
      insert_code(PICLANG_TIME);
      break;
    case PICLANG_FFLUSH:
      write_assembly(assembly_file,"\tfflush\n");
      insert_code(PICLANG_FFLUSH);
      break;
    case PICLANG_FCLEAR:
      write_assembly(assembly_file,"\tfclear\n");
      insert_code(PICLANG_FCLEAR);
      break;
    case PICLANG_CLEAR:
      write_assembly(assembly_file,"\tclear\n");
      insert_code(PICLANG_CLEAR);
      break;
    case PICLANG_MUTEX_LOCK:
      write_assembly(assembly_file,"\tlock\n");
      insert_code(PICLANG_MUTEX_LOCK);
      break;
    case PICLANG_MUTEX_UNLOCK:
      write_assembly(assembly_file,"\tunlock\n");
      insert_code(PICLANG_MUTEX_UNLOCK);
      break;
    case PICLANG_SET_TIME:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tsettime\n");
      insert_code(PICLANG_SET_TIME);
      break;
    case PICLANG_SET_DATE:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tsetdate\n");
      insert_code(PICLANG_SET_DATE);
      break;
    case PICLANG_GETD:
      write_assembly(assembly_file,"\tgetd\n");insert_code(PICLANG_GETD);
      break;
    case PICLANG_GETCH:
      write_assembly(assembly_file,"\tgetch\n");insert_code(PICLANG_GETCH);
      break;
    case PICLANG_NOT:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tnot\n");
      insert_code(PICLANG_NOT);
      break;
    case PICLANG_CHDIR:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tchdir\n");
      insert_code(PICLANG_CHDIR);
      break;
    case PICLANG_PWDIR:
      write_assembly(assembly_file,"\tpwdir\n");
      insert_code(PICLANG_PWDIR);
      break;
    case PICLANG_MOUNT:
      deal_with_arguments(&p->opr);
      write_assembly(assembly_file,"\tmount\n");
      insert_code(PICLANG_MOUNT);
      break;
    default:// all piclang functions
      {
	deal_with_arguments(&p->opr);
	switch(p->opr.oper) {
	case PICLANG_DEREF:// array access
	  write_assembly(assembly_file,"\tderef\n");
	  insert_code(PICLANG_DEREF);
	  break;
	case PICLANG_BSR:
	  write_assembly(assembly_file,"\tbsr \n");
	  insert_code(PICLANG_BSR);
	  break;
	case PICLANG_BSL:
	  write_assembly(assembly_file,"\tbsl \n");
	  insert_code(PICLANG_BSL);
	  break;
	case PICLANG_AND:
	  write_assembly(assembly_file,"\tand\n");
	  insert_code(PICLANG_AND);
	  break;
	case PICLANG_OR:
	  write_assembly(assembly_file,"\tor\n");
	  insert_code(PICLANG_OR);
	  break;
	case PICLANG_MOD:
	  write_assembly(assembly_file,"\tmod \n"); 
	  insert_code(PICLANG_MOD);
	  break;
	case PICLANG_ADD:   
	  write_assembly(assembly_file,"\tadd \n"); 
	  insert_code(PICLANG_ADD);
	  break;
	case PICLANG_SUB:   
	  write_assembly(assembly_file,"\tsub\n");
	  insert_code(PICLANG_SUB); 
	  break; 
	case PICLANG_MULT:   
	  write_assembly(assembly_file,"\tmul\n");
	  insert_code(PICLANG_MULT); 
	  break;
	case PICLANG_DIV:   
	  write_assembly(assembly_file,"\tdiv\n"); 
	  insert_code(PICLANG_DIV);
	  break;
	case PICLANG_COMPLT:   
	  write_assembly(assembly_file,"\tcomplt\n"); 
	  insert_code(PICLANG_COMPLT);
	  break;
	case PICLANG_COMPGT:   
	  write_assembly(assembly_file,"\tcompgt\n"); 
	  insert_code(PICLANG_COMPGT);
	  break;
	case PICLANG_COMPNE:    
	  write_assembly(assembly_file,"\tcompne\n"); 
	  insert_code(PICLANG_COMPNE);
	  break;
	case PICLANG_COMPEQ:    
	  write_assembly(assembly_file,"\tcompeq\n"); 
	  insert_code(PICLANG_COMPEQ);
	  break;
	case PASM_STATEMENT_DELIM:
	  break;
	default:
	  fprintf(stderr,"Unknown op code: %d\n",p->opr.oper);
	  exit(-1);
	  break;
	}//switch
      }//default
    }
  }
  return 0;
}

struct subroutine_map *insert_subroutine(const char *name)
{
  
  if(global_subroutines == NULL)
    global_subroutines = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
  else
    {
      struct subroutine_map* tmp;

      // Search to see if this subroutine exists
      tmp = global_subroutines;
      while(tmp != NULL)
	{
	  if(strcmp(tmp->name,name) == 0)
	    {
	      // already defined here. Check to see if the label is the same
	      // If it is -1, then it was declared by not defined.
	      // If not, this a multiple definition error
		    fprintf(stderr,"Multiple definitions of %s\n",name);
		    exit(-1); 
	    }
	  tmp = tmp->next;
	}
      
      // Not found. Create it.
      tmp = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
      tmp->next = global_subroutines;
      global_subroutines = tmp;
    }
  strcpy(global_subroutines->name,name);
  global_subroutines->address = global_subroutines->size = -1;
  global_subroutines->code = global_subroutines->strings = NULL;
  global_subroutines->code_end = global_subroutines->strings_end = NULL;
  global_subroutines->variables = NULL; 
  return global_subroutines;
}

void free_subroutine(struct subroutine_map *subroutine)
{
	if(subroutine == NULL)
		return;
	free_code(subroutine->code); subroutine->code = (struct compiled_code *)0xdead;
        free_code(subroutine->strings); subroutine->strings = (struct compiled_code *)0xdead;
	free_all_variables(subroutine->variables);subroutine->variables = (idNodeType *)0xdead;
	subroutine->next = (struct subroutine_map *)0xdead;
}

void all_free_subroutines(struct subroutine_map *subroutine)
{
	struct subroutine_map *next = NULL;
	while(subroutine != NULL)
	{
		next = subroutine->next;
		free_subroutine(subroutine);
		subroutine = next;
	}

}
#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

nodeType *full_con(int value, int relocation_type) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;
    p->con.relocation_type = relocation_type;

    return p;
}

nodeType *con(int value){ return full_con(value,-1);}

int handle_string(const char *pStr)
{
  extern FILE *assembly_file;
  int retval = -1;
  if(pStr != NULL)
    {
      int is_new = true;
      retval = resolve_string(pStr,&is_new) + PICLANG_STRING_OFFSET;// when referencing strings, arguments will go first.
      if(is_new)
	{
	  if(assembly_file != NULL)
	    write_assembly(assembly_file,"\t\"%s\"\n", pStr,retval);
	  while(pStr != NULL)
	    {
	      insert_string(*pStr);
	      if(*pStr == 0)
		break;
	      pStr++;
	    }
	}
    }
  else
    {
      yyerror("Invalid string");
    }
  return retval;
}

nodeType *full_id(idNodeType variable_node, bool is_const, int data_type) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = variable_node.i;
    strncpy(p->id.name, variable_node.name,FILENAME_MAX);
    p->id.next = NULL;
    p->id.type = data_type;
    p->id.constant = is_const;
    return p;
}

nodeType *const_id(idNodeType variable_node, bool is_const){return full_id(variable_node, is_const,data_int);}

nodeType *id(idNodeType variable_node) { return const_id(variable_node,false);}

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

void fline_message(FILE *file, char *s)
{
  if(file == NULL)
    file = stdout;
  fprintf(file, "(Line");
  if(yylloc.first_line != yylloc.last_line)
    fprintf(file,"s");
  fprintf(file," %d",yylloc.first_line);
  if(yylloc.first_line != yylloc.last_line)
    fprintf(file," - %d",yylloc.last_line);
  
  fprintf(file,") %s ",s);
}

void yyerror(char *s) {
  extern char *yytext;
  fline_message(stdout, s);
  fprintf(stdout," %s\n",yytext);
  exit(-1);
}


void write_val_for_pic(FILE *binary_file,picos_size_t val)
{
  picos_size_t endiantest = 1;
  int size;
  unsigned char is_littleendian, *end; 
  if(binary_file == NULL)
    return;

  // Ensure little endian is used.
  size = sizeof(val);// in bytes
  end = (char*)&endiantest;
  is_littleendian = *end;
  if(is_littleendian)
    {
      //little endian
      end = (char*)&val;
      while(size > 0)
	{
	  fprintf(binary_file,"%c",*end);
	  end++;
	  size--;
	}
    }
  else
    {
      while(size > 0)
	{
	  fprintf(binary_file,"%c",(val & 0xff));
	  val >> 8;
	  size--;
	}
    }
  
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
	*is_new = true;
      return 0;
    }
  
  if(is_new != NULL)
    *is_new = false;
  i = 0;
  retval = 0;
  for(;i<num_strings;i++)
    if(strcmp(string_list[i],str) == 0)
      return retval;
    else
      retval += strlen(string_list[i]) + 1;
  
  if(is_new != NULL)
    *is_new = true;
  string_list = (char**)realloc(string_list,(num_strings+1)*sizeof(char*));
  string_list[num_strings++] = strdup(str);
  return retval;
}

int count_variables()
{
  int retval = 0;
  const idNodeType *it = NULL;

  if(g_curr_subroutine == NULL)
	return 0;

  it = g_curr_subroutine->variables;
  while(it != NULL)
    {
      retval++;
      it = it->next;
    }
  return retval;
}

void free_variable(idNodeType *variable)
{
	if(variable == NULL)
		return;
	variable->next = (idNodeType*)0xdead;
	free(variable);	
}

void free_all_variables(idNodeType *variable)
{
	idNodeType *tmp;
	
	while(variable != NULL)
	{
		tmp = variable->next;
		free_variable(variable);
		variable = tmp;
	}
}

idNodeType* resolve_variable(const char *name)
{
  int i;
  idNodeType *curr_variable, **variable_list;

  if(g_curr_subroutine == NULL)
  {
	  yyerror("internal error: g_curr_subroutine is not defined.\n");
  }

  curr_variable = g_curr_subroutine->variables;
  if(name == NULL)
    {
      yyerror("Invalid variable name: NULL POINTER\n");
      return NULL;
    }

  if(g_curr_subroutine->variables == NULL)
    {
      g_curr_subroutine->variables = (idNodeType*)malloc(sizeof(idNodeType));
      g_curr_subroutine->variables->i = 0;
      strcpy(g_curr_subroutine->variables->name,name);
      g_curr_subroutine->variables->next = NULL;
      return g_curr_subroutine->variables;
    }
  
  while(curr_variable != NULL)
    {
      if(strcmp(curr_variable->name,name) == 0)
	return curr_variable;
      curr_variable = curr_variable->next;
    }

  // At this point, the variable does not exist.
  curr_variable = (idNodeType*)malloc(sizeof(idNodeType));
  strcpy(curr_variable->name,name);
  curr_variable->i = count_variables();
  curr_variable->next = g_curr_subroutine->variables;
  g_curr_subroutine->variables = curr_variable;

  return curr_variable;
}

struct subroutine_map* get_subroutine(const char *name)
{
  const struct subroutine_map *retval = NULL;
  if(name == NULL)
    {
      fprintf(stderr,"NULL pointer for the subroutine name.\n");
      exit(-1);
    }
  if(global_subroutines == NULL)
    {
      insert_subroutine(name);
    }
  
  retval = global_subroutines;
  while(retval != NULL)
    {
      if(strcmp(retval->name,name) == 0)
	return retval;
      retval = retval->next;
    }
  
  // If here, the subroutine was not yet defined.
  return insert_subroutine(name);
}

void set_pcb_type(struct compiled_code *the_pcb)
{
  if(the_pcb == NULL)
    return;
  the_pcb->type = typePCB;
  set_pcb_type(the_pcb->next);
}

struct compiled_code* MakePCB(struct compiled_code *the_code, struct compiled_code *the_strings, int total_memory, picos_size_t piclang_bitmap)
{
  int i, pad_size;
  static const char name[] = "David";

  struct compiled_code *magic_number = NULL, *first_byte = NULL, *code_index = NULL;
  struct compiled_code *page_size = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *bitmap = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  bitmap->val = piclang_bitmap;
  struct compiled_code *num_pages = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  num_pages->val = (picos_size_t)ceil(1.0*total_memory/PAGE_SIZE);
  struct compiled_code *pc = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *status = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  status->val = PICLANG_SUCCESS;
  struct compiled_code *start_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  start_address->val = FS_BUFFER_SIZE;
  struct compiled_code *string_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *stack, *end_of_stack;
  struct compiled_code *call_stack, *end_of_call_stack;

  if(the_code == NULL)
    {
      fprintf(stderr,"No code to compile!\n");
      exit(-1);
    }

  // Create Stack
  create_stack(&stack,&end_of_stack,PICLANG_STACK_SIZE);
  end_of_stack->next->val = 0;
  end_of_stack = end_of_stack->next;// stack head

  create_stack(&call_stack,&end_of_call_stack,PICLANG_CALL_STACK_SIZE);
  end_of_call_stack->next->val = 0;
  end_of_call_stack = end_of_call_stack->next;// stack head
  
  // Piece the linked list together
  first_byte = page_size;
  page_size->next = bitmap;
  bitmap->next = num_pages;
  num_pages->next = pc;
  pc->next = status;
  status->next = start_address;
  start_address->next = string_address;
  string_address->next = stack;
  end_of_stack->next = call_stack;
  end_of_call_stack->next = NULL;// temporary to count PCB's size and set PCB code types

  // Pad block with zeros
  set_pcb_type(first_byte);
  i = 0;
  pad_size = FS_BUFFER_SIZE - (CountCode(first_byte) + PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t))%FS_BUFFER_SIZE;
  for(;i<pad_size;i++)
    {
      end_of_call_stack->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      end_of_call_stack = end_of_call_stack->next;
      end_of_call_stack->next = NULL;
      end_of_call_stack->type = typePad;
      end_of_call_stack->val = name[i%5];
    }
  start_address->val = (CountCode(first_byte)+PCB_MAGIC_NUMBER_OFFSET)/FS_BUFFER_SIZE + 1;

  // Pad pages to fit into blocks
  page_size->val = FS_BUFFER_SIZE - (FS_BUFFER_SIZE%sizeof(picos_size_t));
  code_index = the_code;// verified to not be null above
  i = sizeof(picos_size_t);
  while(code_index->next != NULL){
    if(i == page_size->val)
	{
	  // End of page, pad buffer.
	  struct compiled_code *next_op = code_index->next;
	  for(;i<FS_BUFFER_SIZE;i++)
	    {
	      code_index->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
	      code_index = code_index->next;
	      code_index->next = NULL;
	      code_index->val = ((i-page_size->val)%2) ? 0xad : 0xde;
	      code_index->type =typePad;
	    }
	  code_index->next = next_op;
	  i = 0;
	  continue;
	}
    code_index = code_index->next;
    i += sizeof(picos_size_t);
    if(code_index == NULL)
      break;
  }
  
  // Pad last block
  for(;i<FS_BUFFER_SIZE;i++)
    {
      code_index->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      code_index = code_index->next;
      code_index->next = NULL;
      code_index->val = ((i-page_size->val)%2) ? 0xad : 0xde;
      code_index->type =typePad;
    }  

  // Attach strings
  end_of_call_stack->next = the_code;
  string_address->val = (CountCode(first_byte)+PCB_MAGIC_NUMBER_OFFSET)/FS_BUFFER_SIZE + 1;

  
  // Find the location of the main function
  pc->val = lookup_label(the_code,((struct subroutine_map*) get_subroutine("main"))->address);
  if(pc->val == (picos_size_t)-1)
	reason_exit("error: Undefined \"main\" function.\n");

  while(the_code->next != NULL)
    the_code = the_code->next;
  if(the_strings != NULL)
    the_code->next = the_strings;
  
  // Magic number to identify the executable
  i = PCB_MAGIC_NUMBER_OFFSET - 1;
  for(;i >= 0;i--)
    {
      magic_number = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      magic_number->next = first_byte;
      magic_number->val = PICLANG_magic_numbers[i];
      magic_number->type = typePCB;
      first_byte = magic_number;
    }
  

  return first_byte;
}

static void dump_code(FILE *eeprom_file,FILE *hex_file,struct compiled_code **the_code)
{
  char hex_buffer[45];
  memset(hex_buffer,0,(9 + COMPILE_MAX_WIDTH + 2)*sizeof(char));// header + data + checksum
  if(hex_file != NULL)
    {
      fprintf(hex_file,":020000040000FA\n");
      FPrintCode(hex_file,*the_code,0,hex_buffer,0x4200,0,PRINT_HEX);
      fprintf(hex_file,":00000001FF\n");
    }
  if(eeprom_file != NULL)
    FPrintCode(eeprom_file,*the_code,0,hex_buffer,0x4200,0,PRINT_EEPROM_DATA);

}

void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct subroutine_map *the_subroutines, picos_size_t *piclang_bitmap)
{
  if(the_subroutines == NULL)
	return;
  resolve_labels(the_subroutines->code);
  dump_code(eeprom_file,hex_file,&the_subroutines->code);
}

void pasm_build(FILE *eeprom_file,FILE *hex_file,struct subroutine_map *the_subroutines, picos_size_t *piclang_bitmap)
{ 
  int num_variables = count_variables(); // REPLACE WITH GLOBAL COUNT
  if(the_subroutines == NULL)
	return;

  pasm_compile(eeprom_file,hex_file,the_subroutines, piclang_bitmap);
  the_subroutines->code = MakePCB(the_subroutines->code,the_subroutines->strings,num_variables,*piclang_bitmap);
  dump_code(eeprom_file,hex_file,&the_subroutines->code);
}

void preprocess(const char *keyword, nodeType *p)
{
  extern FILE *assembly_file;
  write_assembly(assembly_file,";preproc '%s' with type #%d\n",keyword,p->type);
  return;
}

