#include <stdio.h>
#include <stdlib.h>
#include "pasm.h"
#include "pasm_yacc.h"
#include "../piclang.h"

static int lbl;
extern FILE *assembly_file;

void insert_code(unsigned char val)
{
  if(the_code == NULL)
    {
      the_code = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      the_code_end = the_code;
      the_code->val = val;
      the_code->label = 0;
      return;
    }
  
  the_code_end->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  the_code_end->next->label = the_code_end->label + 1;
  the_code_end = the_code_end->next;
  the_code_end->next = NULL;
  the_code_end->val = val;
  
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
      insert_code(p->id.i);
      break;
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
        case PRINT:     
	  ex(p->opr.op[0]);
           fprintf(assembly_file,"\tprint\n");insert_code(PICLANG_PRINT);
            break;
        case PRINTL:
            ex(p->opr.op[0]);
            fprintf(assembly_file,"\tprintl\n");insert_code(PICLANG_PRINTL);
            break;
        case '=':       
            ex(p->opr.op[1]);
            fprintf(assembly_file,"\tpop\t%c\n", p->opr.op[0]->id.i + 'a');insert_code( PICLANG_POP);insert_code(p->opr.op[0]->id.i);
            break;
        case UMINUS:    
            ex(p->opr.op[0]);
            fprintf(assembly_file,"\tneg\n");
            break;
	case INPUT:
	  //ex(p->opr.op[0]);
	  fprintf(assembly_file,"\tpushl\t%c\n",p->opr.op[0]->id.i + 'a');
	  insert_code(PICLANG_PUSHL);
	  insert_code(p->opr.op[0]->id.i);
	  fprintf(assembly_file,"\tinput\n");
	  insert_code( PICLANG_INPUT);
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
