#include "page.h"
#include "picfs_error.h"
#include "picos_time.h"
#include "piclang.h"
#include "arg.h"
#include "io.h"
#include "utils.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <htc.h>
#include <stdio.h>
#include <string.h>

#define PICLANG_error(code)  curr_process.status = code

char PICLANG_load(unsigned int sram_addr)
{
  char size = 0,pos = 0,counter = 0;
  char magic_numbers[PCB_MAGIC_NUMBER_OFFSET];
  if(sram_addr == 0xffff)
    return PICLANG_NO_SUCH_PROGRAM;

  // Verify this is an executable with the magic number
  SRAM_read(sram_addr,magic_numbers,PCB_MAGIC_NUMBER_OFFSET);
  if(strncmp(magic_numbers,PICLANG_magic_numbers,PCB_MAGIC_NUMBER_OFFSET) != 0)
    {
      error_code = PICLANG_INVALID_EXECUTABLE;
      return error_code;
    }
  sram_addr += PCB_MAGIC_NUMBER_OFFSET;

  // Load PCB into memory
  SRAM_read(sram_addr,&curr_process,PCB_SIZE);

  PICLANG_quantum = DEFAULT_PICLANG_QUANTUM;

  if(curr_process.status != PICLANG_SUSPENDED)
    {
      char retval = PAGE_request(curr_process.num_pages,curr_process.start_address);
      if(retval != 0)
	return error_code;
    }

  curr_process_addr = sram_addr;
  return PICLANG_SUCCESS;

}

char PICLANG_save(char saved_status)
{
  char status;
  if(curr_process.size == 0)
    return PICLANG_NO_SUCH_PROGRAM;

  curr_process.status = saved_status;

  SRAM_write(curr_process_addr,&curr_process,PCB_SIZE);

  PICLANG_init();

  return status;

}


void PICLANG_init()
{
  curr_process.size = 0;
  curr_process.pc = 0;
  curr_process.status = PICLANG_SUSPENDED;
  curr_process.start_address = 0;
  curr_process.string_address = 0;
  curr_process.stack_head = 0;
  PICLANG_quantum = 0;
  curr_process_addr = 0xffff;
}

char PICLANG_get_next_byte()
{
  unsigned int next = curr_process_addr + curr_process.pc;
  char val;
  curr_process.pc++;
  next += curr_process.start_address;
  if(next < curr_process_addr)
    {
      PICLANG_error(PICLANG_PC_OVERFLOW);
      return 1;
    }
  SRAM_read(next,&val,1);
  return val;
}

void PICLANG_pushl(char val)
{
  if(curr_process.stack_head >= PICLANG_STACK_SIZE)
    {
      PICLANG_error(PICLANG_STACK_OVERFLOW);
      return;
    }
  curr_process.stack[curr_process.stack_head++] = val;
}

char PICLANG_pop()
{
  if(curr_process.stack_head > PICLANG_STACK_SIZE)
    {
      PICLANG_error(PICLANG_STACK_OVERFLOW);
      return 0;
    }
  return curr_process.stack[--curr_process.stack_head];
}

void PICLANG_next()
{
  char command;
  if(curr_process.size == 0)
    return;

  if(PICLANG_quantum == 0)
    {
      PICLANG_save(PICLANG_SUSPENDED);
      return;
    }
  
  command = PICLANG_get_next_byte();
  if(curr_process.status != PICLANG_SUCCESS)
    return;
  switch(command)
    {
    case EOP:
      PICLANG_save(SUCCESS);
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
    case PICLANG_PUSH:
      {
	char val = PAGE_get(PICLANG_get_next_byte(),0/* replace with UID */);
	if(error_code != SUCCESS)
	  PICLANG_error(error_code);
	else
	  PICLANG_pushl(val);
	break;
      }
    case PICLANG_POP:
      {
	char addr = PICLANG_get_next_byte();
	PAGE_set(addr,PICLANG_pop(),0/* replace with UID */);
	break;
      }
    case PICLANG_PRINT:
      {
	putch(PICLANG_pop());
	IO_flush();
	break;
      }
    case PICLANG_PRINTL:
      IO_putd(PICLANG_pop());
      IO_flush();
      break;
    case PICLANG_SPRINT:
      {
	char ch;
	unsigned int addr;
	static bit should_flush;
	addr = curr_process_addr + curr_process.string_address;
	addr += PICLANG_pop();
	SRAM_read(addr++,&ch,1);
	if(ch == 0)
	  should_flush = FALSE;
	else
	  should_flush = TRUE;
	while(ch != 0)
	  {
	    putch(ch);
	    SRAM_read(addr++,&ch,1);
	  }
	if(should_flush == TRUE)
	  IO_flush();
	break;
      }
    case PICLANG_SYSTEM:
      PICLANG_system = PICLANG_pop();
      PICLANG_quantum = 0;// will suspend for system call
      break;
    case PICLANG_MORSE:
      {
	char two[2];
	unsigned int addr;
	two[1] = 0;
	addr = PICLANG_pop();
	SRAM_read(addr++,two,1);
	while(two[0] != 0)
	  {
	    morse_sound(two);
	    SRAM_read(addr++,two,1);
	  }
	break;
      }
    case PICLANG_TIME:
      TIME_stdout();
      break;
    case PICLANG_SET_TIME:case PICLANG_SET_DATE:
      {
	TIME_t *newtime = TIME_get();
	if(newtime == NULL)
	  {
	    PICLANG_error(PICLANG_NULL_POINTER);
	    return;
	  }
	if(command == PICLANG_SET_TIME)
	  {
	    newtime->minutes = PICLANG_pop();
	    newtime->hours = PICLANG_pop();
	    if(newtime->minutes > 59 || newtime->hours > 23)
	      {
		newtime->minutes = newtime->hours = 0;
		PICLANG_error(TIME_INVALID);
	      }
	  }
	else
	  {
	    newtime->day = PICLANG_pop();
	    newtime->month = PICLANG_pop();
	    if(newtime->month > 12 || newtime->day > 31)
	      {
		newtime->month = newtime->day = 0;
		PICLANG_error(TIME_INVALID);
	      }
	  }
	break;
      }
    case PICLANG_ARGD:
      {
	signed char argd = ARG_getd();
	if(argd < 0)
	  {
	    curr_process.status = error_code;
	    error_code = 0;
	    return;
	  }
	PICLANG_pushl((char)argd);
	break;
      }
    case PICLANG_NUM_COMMANDS:default:
      PICLANG_error(PICLANG_UNKNOWN_COMMAND);
      return;
    }
}
