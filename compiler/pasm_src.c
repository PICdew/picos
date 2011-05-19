#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pasm.h"
#include "pasm_yacc.h"
#include "../piclang.h"

static int lbl;
extern FILE *assembly_file;
void insert_compiled_code(struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, unsigned char val);

void insert_code(unsigned char val)
{
  insert_compiled_code(&the_code,&the_code_end,val);
}

void insert_string(unsigned char val)
{
  insert_compiled_code(&the_strings,&the_strings_end,val);
}


void insert_compiled_code(struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, unsigned char val)
{
  struct compiled_code *list = *ptrlist;
  struct compiled_code *list_end = *ptrlist_end;
  if(list == NULL)
    {
      *ptrlist = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      *ptrlist_end = *ptrlist;
      list = *ptrlist;
      list->val = val;
      list->label = 0;
      list->next = NULL;
      return;
    }
  
  list_end->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  list_end->next->label = list_end->label + 1;
  *ptrlist_end = list_end->next;
  (*ptrlist_end)->next = NULL;
  (*ptrlist_end)->val = val;
  
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

int ex(nodeType *p) {
    int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
      fprintf(assembly_file,"\tpushl\t%d\n", p->con.value); 
      insert_code(PICLANG_PUSHL);
      insert_code(p->con.value);
      break;
    case typeId:        
      fprintf(assembly_file,"\tpush\t%c\n", p->id.i + 'a');
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
		fprintf(assembly_file,"\tstore\t\"%s\"\n", pStr);
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
            fprintf(assembly_file,"L%03d:\n", lbl1 = lbl++);
            ex(p->opr.op[0]);
            fprintf(assembly_file,"\tjz\tL%03d\n", lbl2 = lbl++);
            ex(p->opr.op[1]);
            fprintf(assembly_file,"\tjmp\tL%03d\n", lbl1);
            fprintf(assembly_file,"L%03d:\n", lbl2);
            break;
        case IF:
            ex(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                fprintf(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                fprintf(assembly_file,"\tjmp\tL%03d\n", lbl2 = lbl++);
                fprintf(assembly_file,"L%03d:\n", lbl1);
                ex(p->opr.op[2]);
                fprintf(assembly_file,"L%03d:\n", lbl2);
            } else {
                /* if */
                fprintf(assembly_file,"\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                fprintf(assembly_file,"L%03d:\n", lbl1);
            }
            break;
        case PUTD:     
	  ex(p->opr.op[0]);
           fprintf(assembly_file,"\tputd\n");insert_code(PICLANG_PRINTL);
            break;
        case PUTCH:
            ex(p->opr.op[0]);
            fprintf(assembly_file,"\tputch\n");insert_code(PICLANG_PRINT);
            break;
        case '=':       
            ex(p->opr.op[1]);
            fprintf(assembly_file,"\tpop\t%c\n", p->opr.op[0]->id.i + 'a');insert_code( PICLANG_POP);insert_code(resolve_variable(p->opr.op[0]->id.i));
            break;
        case UMINUS:    
            ex(p->opr.op[0]);
            fprintf(assembly_file,"\tneg\n");
            break;
	case INPUT:
	  //ex(p->opr.op[0]);
	  fprintf(assembly_file,"\tpushl\t%c\n",p->opr.op[0]->id.i + 'a');
	  insert_code(PICLANG_PUSHL);
	  insert_code(resolve_variable(p->opr.op[0]->id.i));
	  fprintf(assembly_file,"\tinput\n");
	  insert_code( PICLANG_INPUT);
	  break;
        case SYSTEM:
	  {
	    int op_counter = p->opr.nops - 1;
	    for(;op_counter >= 0 ;op_counter--)
		ex(p->opr.op[op_counter]);
	    fprintf(assembly_file,"\tsystem\n");insert_code(PICLANG_SYSTEM);
	    break;
	  }
	case SPRINT:
	  ex(p->opr.op[0]);
	  fprintf(assembly_file,"\tsprint\n");
	  insert_code(PICLANG_SPRINT);
	  break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   fprintf(assembly_file,"\tadd \n"); insert_code(PICLANG_ADD);break;
            case '-':   fprintf(assembly_file,"\tsub\n");insert_code(PICLANG_SUB); break; 
            case '*':   fprintf(assembly_file,"\tmul\n");insert_code(PICLANG_MULT); break;
            case '/':   fprintf(assembly_file,"\tdiv\n"); break;
            case '<':   fprintf(assembly_file,"\tcompLT\n"); break;
            case '>':   fprintf(assembly_file,"\tcompGT\n"); break;
            case GE:    fprintf(assembly_file,"\tcompGE\n"); break;
            case LE:    fprintf(assembly_file,"\tcompLE\n"); break;
            case NE:    fprintf(assembly_file,"\tcompNE\n"); break;
            case EQ:    fprintf(assembly_file,"\tcompEQ\n"); break;
            }
        }
    }
    return 0;
}
