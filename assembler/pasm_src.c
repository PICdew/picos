#include <stdio.h>
#include <stdlib.h>
#include "pasm.h"
#include "pasm_yacc.h"
#include "../piclang.h"

static int lbl;

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
      printf("\tpushl\t%d\n", p->con.value); 
      insert_code(PICLANG_PUSHL);
      insert_code(p->con.value);
      break;
    case typeId:        
      printf("\tpush\t%c\n", p->id.i + 'a');
      insert_code(p->id.i);
      break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            printf("L%03d:\n", lbl1 = lbl++);
            ex(p->opr.op[0]);
            printf("\tjz\tL%03d\n", lbl2 = lbl++);
            ex(p->opr.op[1]);
            printf("\tjmp\tL%03d\n", lbl1);
            printf("L%03d:\n", lbl2);
            break;
        case IF:
            ex(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                printf("\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                printf("L%03d:\n", lbl1);
                ex(p->opr.op[2]);
                printf("L%03d:\n", lbl2);
            } else {
                /* if */
                printf("\tjz\tL%03d\n", lbl1 = lbl++);
                ex(p->opr.op[1]);
                printf("L%03d:\n", lbl1);
            }
            break;
        case PRINT:     
            ex(p->opr.op[0]);
            printf("\tprint\n");insert_code(PICLANG_PRINT);
            break;
        case PRINTL:
            ex(p->opr.op[0]);
            printf("\tprintl\n");insert_code(PICLANG_PRINTL);
            break;
        case '=':       
            ex(p->opr.op[1]);
            printf("\tpop\t%c\n", p->opr.op[0]->id.i + 'a');insert_code( PICLANG_POP);
            break;
        case UMINUS:    
            ex(p->opr.op[0]);
            printf("\tneg\n");
            break;
        default:
            ex(p->opr.op[0]);
            ex(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   printf("\tadd \n"); insert_code(PICLANG_ADD);break;
            case '-':   printf("\tsub\n");insert_code(PICLANG_SUB); break; 
            case '*':   printf("\tmul\n");insert_code(PICLANG_MULT); break;
            case '/':   printf("\tdiv\n"); break;
            case '<':   printf("\tcompLT\n"); break;
            case '>':   printf("\tcompGT\n"); break;
            case GE:    printf("\tcompGE\n"); break;
            case LE:    printf("\tcompLE\n"); break;
            case NE:    printf("\tcompNE\n"); break;
            case EQ:    printf("\tcompEQ\n"); break;
            }
        }
    }
    return 0;
}
