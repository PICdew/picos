/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 20 June 2012
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * Functions shared between multiple programs in the compiler suite.
 */

#include "picos/tools/pasm.h"
#include "picos/piclang.h"
#include "picos/page.h"
#include "picos/version.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

extern FILE *assembly_file;
const char PICLANG_LIB_MAGIC_NUMBERS[] = "PICLIB";
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

void free_all_code(struct compiled_code *code_list)
{
	struct compiled_code *tmp;

	while(code_list != NULL)
	{
		tmp = code_list->next;
		free(code_list);
		code_list = tmp;
	}
	
}

void free_code(struct compiled_code *code)
{
  if(code == NULL)
    return;
  free_code(code->next);
  free(code);
}

void free_subroutine(struct subroutine_map *subroutine)
{
	if(subroutine == NULL)
		return;
	free_code(subroutine->code); subroutine->code = (struct compiled_code *)0xdead;
        free_code(subroutine->strings); subroutine->strings = (struct compiled_code *)0xdead;
	free_all_variables(subroutine->variables);subroutine->variables = (idNodeType *)0xdead;
	subroutine->next = (struct subroutine_map *)0xdead;
    free(subroutine);
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

struct compiled_code* insert_compiled_code(nodeEnum type, struct subroutine_map *subroutine, picos_size_t val, picos_size_t label)
{
  struct compiled_code **ptrlist,**ptrlist_end;
  struct compiled_code *list,*list_end;

  if(subroutine == NULL)
  {
	  fprintf(stderr,"insert_compiled_code: NULL pointer for subroutine object.\n");
	  exit(1);
  }

  switch(type)
  {
	  case typeStr:
		  list = subroutine->strings;
		  list_end = subroutine->strings_end;
		  ptrlist = &subroutine->strings;
		  ptrlist_end = &subroutine->strings_end;
		  break;
	  default: 
		  list = subroutine->code;
		  list_end = subroutine->code_end;
		  ptrlist = &subroutine->code;
		  ptrlist_end = &subroutine->code_end;
		  break;
  }
   
  if(list == NULL)
    {
      *ptrlist = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      *ptrlist_end = *ptrlist;
      list = *ptrlist;
      list->val = val;
      list->label = label;
      list->type = type;
      list->target = NULL;
      list->relocation_type = -1;
      list->is_static = false;
      list->next = NULL;
      return list;
    }
  
  list_end->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  list_end->next->label = label;
  *ptrlist_end = list_end->next;
  (*ptrlist_end)->next = NULL;
  (*ptrlist_end)->val = val;
  (*ptrlist_end)->type = type;
  (*ptrlist_end)->target = NULL;
  (*ptrlist_end)->relocation_type = -1;
  (*ptrlist_end)->is_static = false;
  return *ptrlist_end;
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

void FPrintCode(FILE *hex_file, const struct compiled_code* code, int col, char *buffer,int start_address, int checksum, int print_type)
{
  if(code == NULL || buffer == NULL || hex_file == NULL)
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

size_t CountCode(const struct compiled_code *the_code)
{
  if(the_code == NULL)
    return 0;
  if(the_code->type == typeStr || the_code->type == typePad)
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
  {"lsof", PICLANG_LSOF, 0},
  {"lsmount", PICLANG_LSMOUNT, 0},
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
  {"call",PICLANG_CALL,1},
  {"readdir",PICLANG_READDIR,1},
  {"sprintn",PICLANG_SPRINTN,2},
  {"fstat",PICLANG_FSTAT,1},
  {"move", PICLANG_MOVE, 0},
  {"getx", PICLANG_GETX, 0},
  {"gety", PICLANG_GETY, 0},
  {"rawload", PICLANG_RAWLOAD,0},
  {"kversion", PICLANG_KVERSION,0},
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


void reason_exit_vargs(va_list args, const char *format)
{
  vfprintf(stderr,format,args);
  va_end(args);
  if(errno != 0)
  {
      fprintf(stderr,"Reason (%d): %s\n",errno,strerror(errno));
      exit(errno);
  }
  exit(1);
}

void reason_exit(const char *format, ...)
{
  va_list args;
  va_start(args,format);
  reason_exit_vargs(args,format);
}

void full_assert(int condition,const char *format,...)
{
    va_list args;
    if(condition)
        return;
    va_start(args,format);
    reason_exit_vargs(args,format);
}





