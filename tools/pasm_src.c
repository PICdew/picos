#include "pasm.h"
#include "picosc_yacc.h"
#include "../piclang.h"
#include "page.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

extern FILE *assembly_file;

int write_assembly(FILE *stream, const char *format, ...)
{
  va_list va;
  int retval = 0;

  if(stream != NULL)
    {
      va_start(va,format);
      retval = vfprintf(stream,format,va);
      va_end(va);
    }
  return retval;
}

#if 0
void _attach_label(struct compiled_code *ptrlist_end, picos_size_t label)
{
  if(ptrlist_end == NULL)
    return;
  ptrlist_end->label = label;
}
#endif

void insert_compiled_code(nodeEnum type, struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, picos_size_t val, picos_size_t label)
{
  struct compiled_code *list = *ptrlist;
  struct compiled_code *list_end = *ptrlist_end;
  if(list == NULL)
    {
      *ptrlist = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      *ptrlist_end = *ptrlist;
      list = *ptrlist;
      list->val = val;
      list->label = label;
      list->type = type;
      list->next = NULL;
      return;
    }
  
  list_end->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  list_end->next->label = label;
  *ptrlist_end = list_end->next;
  (*ptrlist_end)->next = NULL;
  (*ptrlist_end)->val = val;
  (*ptrlist_end)->type = type;
}


void FPrintCode(FILE *hex_file,struct compiled_code* code, int col, char *buffer,int start_address, int checksum, int print_type)
{
  if(code == NULL)
    return;

  switch(print_type)
    {
    case PRINT_EEPROM_DATA:
      sprintf(&buffer[5*col],",0x%02x",code->val & 0xff);
      break;
    case PRINT_HEX:default:
      sprintf(&buffer[4*col],"%04x",(code->val << 8) & 0xff00);
      break;
    }
  checksum += (code->val & 0xff);
  col++;
  if(col >= COMPILE_MAX_WIDTH || code->next == NULL)
    {
      checksum += (2*col & 0xff) + (start_address & 0xff) + ((start_address & 0xff00) >> 8);
      checksum = (~checksum & 0xff);
      checksum++;
      switch(print_type)
	{
	case PRINT_EEPROM_DATA:
	  {
	    int counter = 0;
	    fprintf(hex_file,"__EEPROM_DATA(");
	    if(strlen(buffer) > 1)
	      fprintf(hex_file,"%s",&buffer[1]);
	    if(col % 8 != 0)
	      {
		for(;counter < 8-(col % 8);counter++)
		  fprintf(hex_file,",0xff");
	      }
	    fprintf(hex_file,");\n");
	    break;
	  }
	case PRINT_HEX:default:
	  fprintf(hex_file,":%02x%04x00%s%02x\n",col*2,start_address,buffer,checksum);
	  break;
	}
      memset(buffer,0,45*sizeof(char));
      col = 0;
      checksum = 0;
      start_address += 0x10;
    }
    
  
  FPrintCode(hex_file,code->next,col,buffer,start_address,checksum,print_type);
}


const struct subroutine_map* lookup_subroutine(int index)
{
  extern struct subroutine_map *subroutines;
  const struct subroutine_map *retval = subroutines;
  while(retval != NULL)
    {
      if(retval->index == index)
	break;
      retval = retval->next;
    }
  
  return retval;
}

int lookup_label(const struct compiled_code* code, picos_size_t label)
{
  int code_counter = 0;
  if(code == NULL)
    return -1;
  for(;code != NULL;code = code->next)
    {
      if(code->type == typePCB)
	continue;
      if(code->type == typeLabel)
	{
	  if(code->label == label)
	    return code_counter;
	}
      code_counter++;
    }
  return -1;
}

void resolve_labels(struct compiled_code* code)
{
  const struct compiled_code* code_head = code;
  if(code == NULL)
    return;
  
  for(;code != NULL;code = code->next)
    {
      switch(code->val)
	{
	case PICLANG_JMP:case PICLANG_JZ:case PICLANG_CALL:// change labels to addresses
	  if(code->next != NULL)
	    {
	      int label_addr, subroutine_label;
	      // Next next word should contain the target address.
	      // 1.) If the type of the next word is a subroutine, the label
	      //     of the subroutine must be found. That label is used
	      //     to find the target *address*.
	      // 2.) Otherwise the next byte is the label itself, which is
	      //     used to find the target *address*.
	      if(code->next->val == PASM_SUBROUTINE)
		{
		  const struct subroutine_map *subroutine = lookup_subroutine(code->next->label);
		  if(subroutine == NULL || subroutine->label == -1)
		    {
		      if(subroutine == NULL)
			fprintf(stderr,"Undefined reference: #%d\n",code->next->label);
		      else
			fprintf(stderr,"Undefined reference: %s\n",subroutine->name);
		      exit(-1);
		    }
		  label_addr = lookup_label(code_head, subroutine->label);
		}
	      else
		label_addr = lookup_label(code_head, code->next->label);
	      if(label_addr < 0)
		{
		  fprintf(stderr,"Could not resolve label %d\n",code->next->label);
		  return;
		}
	      code->next->val = (picos_size_t)label_addr;
	      code = code->next;
	      continue;
	    }
	  break;
	case PICLANG_SIGNAL:
	  if(code->next != NULL && code->next->next != NULL)
	    {
	      int label_addr = lookup_label(code_head, code->next->next->val);
	      if(label_addr < 0)
		{
		  fprintf(stderr,"Could not resolve label %d\n",code->next->next->val);
		  return;
		}
	      code->next->next->val = (picos_size_t)label_addr;
	      code = code->next->next;
	      continue;
	    }
	  break;
	default:
	  break;
	}
      if((opcode2assembly(code->val))->has_arg)
	code = code->next;
    }
}

/**
 * Creates a stack, with the stack head pointer at the end.
 * The size (stack_size) is the number STACK ELEMENTS. Thus,
 * pstack will point to a linked list of stack_size+1 elements,
 * where the last element is the stack head pointer.
 * pstack_end will point to the last stack element, not the stack head.
 * stack head is at (*pstack_end)->next
 */
void create_stack(struct compiled_code **pstack, struct compiled_code **pstack_end, size_t stack_size)
{
  struct compiled_code *stack, *end_of_stack;
  if(pstack == NULL || pstack_end == NULL)
    return;
  stack = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  end_of_stack = stack;
  stack->val = 0xff;

  size_t stack_counter = 1;
  for(;stack_counter < stack_size;stack_counter++)
    {
      end_of_stack->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      end_of_stack = end_of_stack->next;
      end_of_stack->val = 0xff;
    }
  end_of_stack->next  = (struct compiled_code*)malloc(sizeof(struct compiled_code));// this is the stack head pointer.
  
  *pstack = stack;
  *pstack_end = end_of_stack;
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
  if(the_code->type == typeStr)
    return sizeof(char) + CountCode(the_code->next);
  else
    return sizeof(picos_size_t) + CountCode(the_code->next);
}

struct assembly_map opcodes[] = {
  {"mod", PICLANG_MOD, 0},
  {"add", PICLANG_ADD, 0},
  {"sub", PICLANG_SUB, 0}, 
  {"mult", PICLANG_MULT, 0},
  {"div", PICLANG_DIV, 0},
  {"bsl", PICLANG_BSL, 0},
  {"bsr", PICLANG_BSR, 0},
  {"and", PICLANG_AND, 0},
  {"or", PICLANG_OR, 0},
  {"not", PICLANG_NOT, 0},
  {"pushl", PICLANG_PUSHL, 1},
  {"push", PICLANG_PUSH, 1},
  {"pop", PICLANG_POP, 1},
  {"drop", PICLANG_DROP, 0},
  {"swap", PICLANG_SWAP, 0},
  {"putd", PICLANG_PRINT, 0},
  {"putch", PICLANG_PRINTL, 0},
  {"clear", PICLANG_CLEAR, 0},
  {"sprint", PICLANG_SPRINT, 0},
  {"fputch", PICLANG_FPUTCH, 0},
  {"fputd", PICLANG_FPUTD, 0},
  {"fflush", PICLANG_FFLUSH, 0},
  {"fclear", PICLANG_FCLEAR, 0},
  {"fopen", PICLANG_FOPEN, 0},
  {"fclose", PICLANG_FCLOSE, 0},
  {"fread", PICLANG_FREAD, 0},
  {"system", PICLANG_SYSTEM, 0},
  {"signal", PICLANG_SIGNAL, 2},
  {"sleep", PICLANG_SLEEP, 0},
  {"morse", PICLANG_MORSE, 0},
  {"time", PICLANG_TIME, 0},
  {"settime", PICLANG_SET_TIME, 0},
  {"setdate", PICLANG_SET_DATE, 0},
  {"getch", PICLANG_GETCH, 0},
  {"getd", PICLANG_GETD, 0},
  {"jz", PICLANG_JZ, 1},
  {"jmp", PICLANG_JMP, 1},
  {"complt", PICLANG_COMPLT, 0},
  {"compgt", PICLANG_COMPGT, 0},
  {"compeq", PICLANG_COMPEQ, 0},
  {"compne", PICLANG_COMPNE, 0},
  {"exit", PICLANG_EXIT, 0},
  {"chdir", PICLANG_CHDIR, 0},
  {"getdir", PICLANG_PWDIR, 0},
  {"mount", PICLANG_MOUNT, 2},
  {"lock",PICLANG_MUTEX_LOCK,0},
  {"unlock",PICLANG_MUTEX_UNLOCK,0},
  {"ENDofCMDS",PICLANG_NUM_COMMANDS, 0}  
};

struct assembly_map* keyword2assembly(const char *keyword)
{
  struct assembly_map *curr_opcode;
  if(keyword == NULL)
    return NULL;
  if(opcodes == NULL)
    return NULL;
  
  curr_opcode = opcodes;
  while(curr_opcode != NULL && curr_opcode->opcode != PICLANG_NUM_COMMANDS)
    {
      if(strcmp(curr_opcode->keyword,keyword) == 0)
	break;
      curr_opcode++;
    }
  return curr_opcode;
}

struct assembly_map* opcode2assembly(int opcode)
{
  struct assembly_map *curr_opcode;
  if(opcodes == NULL)
    return NULL;
  
  curr_opcode = opcodes;
  while(curr_opcode != NULL && curr_opcode->opcode != PICLANG_NUM_COMMANDS)
    {
      if(curr_opcode->opcode == opcode)
	break;
      curr_opcode++;
    }
  return curr_opcode;
}








