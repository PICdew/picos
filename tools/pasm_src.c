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

void pasm_compile(FILE *eeprom_file,FILE *hex_file,struct compiled_code **the_code, struct compiled_code *the_strings, unsigned char *piclang_bitmap, int num_variables)
{
  char hex_buffer[45];
  
  FirstPass(the_code,FALSE,&piclang_bitmap,num_variables);
  *the_code = MakePCB(*the_code,the_strings,num_variables,piclang_bitmap);
  memset(hex_buffer,0,(9 + COMPILE_MAX_WIDTH + 2)*sizeof(char));// header + data + checksum
  fprintf(hex_file,":020000040000FA\n");
  FPrintCode(hex_file,*the_code,0,hex_buffer,0x4200,0,PRINT_HEX);
  fprintf(hex_file,":00000001FF\n");
  FPrintCode(eeprom_file,*the_code,0,hex_buffer,0x4200,0,PRINT_EEPROM_DATA);

}

void FirstPass(struct compiled_code* code,int skip_assignment_check, unsigned char *piclang_bitmap,  int num_variables)
{
  int clean_skip_assignment_check = skip_assignment_check;
  
  if(code == NULL)
    return;
    switch(code->val)
    {
     case PICLANG_SYSTEM:
      if(piclang_bitmap != NULL)
	*piclang_bitmap |= PICLANG_BIT_SYSCALL;
      break;
    }
  if(clean_skip_assignment_check)
    skip_assignment_check = FALSE;
  
  FirstPass(code->next,skip_assignment_check,piclang_bitmap,num_variables);
}

struct compiled_code* MakePCB(struct compiled_code *the_code, struct compiled_code *the_strings, int total_memory, unsigned char piclang_bitmap)
{
  int i;
  struct compiled_code *magic_number = NULL;
  struct compiled_code *size = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *bitmap = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  bitmap->val = piclang_bitmap;
  struct compiled_code *num_pages = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  num_pages->val = (unsigned char)ceil(1.0*total_memory/PAGE_SIZE);
  struct compiled_code *pc = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  pc->val = 0;
  struct compiled_code *status = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  status->val = PICLANG_SUCCESS;
  struct compiled_code *start_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  start_address->val = PCB_SIZE;
  struct compiled_code *string_address = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  struct compiled_code *stack = (struct compiled_code*)malloc(sizeof(struct compiled_code));
  stack->val = 0xff;
  struct compiled_code *end_of_stack = stack;
  size_t stack_counter = 1;
  for(;stack_counter < PICLANG_STACK_SIZE;stack_counter++)
    {
      end_of_stack->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      end_of_stack = end_of_stack->next;
      end_of_stack->val = 0xff;
    }
  end_of_stack->next  = (struct compiled_code*)malloc(sizeof(struct compiled_code));// this is the stack head pointer.
  end_of_stack->next->val = 0;
  end_of_stack = end_of_stack->next;
  size->next = bitmap;
  bitmap->next = num_pages;
  num_pages->next = pc;
  pc->next = status;
  status->next = start_address;
  start_address->next = string_address;
  string_address->next = stack;
  end_of_stack->next = NULL;// temporary to count PCB's size
  start_address->val = CountCode(size);

  end_of_stack->next = the_code;
  string_address->val = CountCode(size);

  if(the_code == NULL)
    {
      fprintf(stderr,"No code to compile!\n");
      exit -1;
    }
  while(the_code->next != NULL)
    the_code = the_code->next;
  if(the_strings != NULL)
    the_code->next = the_strings;
  else
    {
      the_code->next = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      the_code->next->val = 0;
    }
  
  size->val =  CountCode(size);
  
  // Magic number to identify the executable
  i = PCB_MAGIC_NUMBER_OFFSET - 1;
  for(;i >= 0;i--)
    {
      magic_number = (struct compiled_code*)malloc(sizeof(struct compiled_code));
      magic_number->next = size;
      magic_number->val = PICLANG_magic_numbers[i];
      size = magic_number;
    }

  return size;
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


