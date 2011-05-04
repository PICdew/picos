#include <htc.h>
#include <stdio.h>
#include "piclang.h"

char PICLANG_load(char nth)
{
  char size = 0,pos = 0,counter = 0;
  
  for(;pos < 0xff;)
    {
      size = eeprom_read(pos);
      if(counter == nth)
	break;
      pos = pos + size + PCB_SIZE;// move to beginning of next PCB
      counter++;
    }

  if(size == 0)
    return NO_SUCH_PROGRAM;

  curr_process.size = size;
  curr_process.pc = eeprom_read(++pos);
  curr_process.status = eeprom_read(++pos);
  curr_process.start_address = eeprom_read(++pos);

  counter = 0;
  for(;counter<PICLANG_STACK_SIZE;counter++)
    curr_process.stack[counter] = eeprom_read(++pos);
  curr_process.stack_head = eeprom_read(++pos);

  PICLANG_quantum = DEFAULT_PICLANG_QUANTUM;
  return SUCCESS;

}

char PICLANG_save()
{
  char pos,status;
  if(curr_process.size == 0)
    return NO_SUCH_PROGRAM;

  pos = curr_process.start_address;
  if(pos - 5 - PICLANG_STACK_SIZE < 0)
    return EEPROM_OVERFLOW;
  
  pos--;// move back to stack head
  eeprom_write(pos--,curr_process.stack_head);
  status = PICLANG_STACK_SIZE - 1;
  for(;;status--)// save the stack
    {    
      eeprom_write(pos--,curr_process.stack[status]);
      if(status == 0)
	break;
    }
  pos--;// skip start_address
  if(curr_process.status == SUCCESS)
    curr_process.status = SUSPENDED;
  status = curr_process.status;
  eeprom_write(pos,curr_process.status);
  pos--;
  eeprom_write(pos,curr_process.pc);
  

  PICLANG_init();

  return status;

}


void PICLANG_init()
{
  curr_process.size = 0;
  curr_process.pc = 0;
  curr_process.status = SUSPENDED;
  curr_process.start_address = 0;
  curr_process.stack_head = 0;
  PICLANG_quantum = 0;
}

char PICLANG_get_next_byte(){
  return eeprom_read((curr_process.pc++) + curr_process.start_address);
}

#define PICLANG_error(code)  curr_process.status = code

void PICLANG_pushl(char val)
{
  curr_process.stack_head++;
  if(curr_process.stack_head > PICLANG_STACK_SIZE)
    {
      PICLANG_error(STACK_OVERFLOW);
      return;
    }
  curr_process.stack[curr_process.stack_head] = val;
}

char PICLANG_pop()
{
  if(curr_process.stack_head > PICLANG_STACK_SIZE)
    {
      PICLANG_error(STACK_OVERFLOW);
      return 0;
    }
  return curr_process.stack[curr_process.stack_head--];
}

char* PAGE_resolve(char pageloc)
{
  return 0;
}

void PICLANG_next()
{
  char command;
  if(curr_process.size == 0)
    return;

  if(PICLANG_quantum == 0)
    {
      PICLANG_save();
      return;
    }
  
  if(curr_process.size < curr_process.pc)
    {
      PICLANG_error(PC_OVERFLOW);
      return;
    }

  command = PICLANG_get_next_byte();
  switch(command)
    {
    case EOP:
      PICLANG_save();
      return;
    case PICLANG_ADD:
      PICLANG_pushl(PICLANG_pop() + PICLANG_pop());
      return;
    case PICLANG_SUB:
      PICLANG_pushl(PICLANG_pop() - PICLANG_pop());
      return;
    case PICLANG_MULT:
      PICLANG_pushl(PICLANG_pop() * PICLANG_pop());
      return;
    case PICLANG_PUSHL:
      PICLANG_pushl(PICLANG_get_next_byte());
      break;
    case PICLANG_POP:
      {
	char *addr = PAGE_resolve(PICLANG_get_next_byte());
	if(addr != 0)
	  *addr = PICLANG_pop();
	break;
      }
    case PICLANG_PRINT:
      {
	char two_chrs[2];two_chrs[1] = 0;
	two_chrs[0] = PICLANG_pop();
	printf("%s",two_chrs);
	break;
      }
    case PICLANG_PRINTL:
      printf("%d",PICLANG_pop());
      break;
    case PICLANG_NUM_COMMANDS:default:
      PICLANG_error(UNKNOWN_COMMAND);
      return;
    }
}
