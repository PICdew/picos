#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "pasm.h"
#include "pasm_yacc.h"
#include "../piclang.h"
#include "page.h"

extern FILE *assembly_file;


int write_assembly(FILE *stream, const char *format, ...)
{
  va_list va;
  int retval;
  if(stream == NULL)
    return 0;
  va_start(va,format);
  retval = vfprintf(stream,format,va);
  va_end(va);
  return retval;
}

#if 0
void _attach_label(struct compiled_code *ptrlist_end, unsigned char label)
{
  if(ptrlist_end == NULL)
    return;
  ptrlist_end->label = label;
}
#endif

void insert_compiled_code(nodeEnum type, struct compiled_code** ptrlist, struct compiled_code** ptrlist_end, unsigned char val)
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
      list->type = type;
      list->next = NULL;
      return;
    }
  
  list_end->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  list_end->next->label = list_end->label + 1;
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

int lookup_label(const struct compiled_code* code, unsigned char label)
{
  int label_index = 0;
  if(code == NULL)
    return -1;
  for(;code != NULL;code = code->next)
    {
      if(code->type == typeLabel)
	{
	  if(label_index == label)
	    return code->label;
	  label_index++;
	}
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
	      int label_addr = lookup_label(code_head, code->next->val);
	      if(label_addr < 0)
		{
		  fprintf(stderr,"Could not resolve label %d\n",code->next->val);
		  return;
		}
	      code->next->val = (unsigned char)label_addr;
	      code = code->next;
	      continue;
	    }
	  break;
	default:
	  break;
	}
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
  return 1 + CountCode(the_code->next);
}


