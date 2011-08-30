#include "page.h"
#include "picfs_error.h"
#include "picfs.h"
extern FS_Unit picfs_buffer[];
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
int PICLANG_next_process[] = {0xffff};
char PICLANG_file_buffer_index;

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
      char retval = PAGE_request(curr_process.num_pages,0 /* replace with UID or pid */);
      PICLANG_file_buffer_index = 0;
      if(retval != 0)
	return error_code;
    }

  curr_process_addr = sram_addr;
  return PICLANG_SUCCESS;

}

char PICLANG_save(char saved_status)
{
  if(curr_process.size == 0)
    return PICLANG_NO_SUCH_PROGRAM;

  curr_process.status = saved_status;
  if(saved_status == PICLANG_SUSPENDED)
    PICLANG_next_process[0] = curr_process_addr - PCB_MAGIC_NUMBER_OFFSET;
  else
    {
      PAGE_free(0);
      PICLANG_next_process[0] = 0xffff;
    }


  SRAM_write(curr_process_addr,&curr_process,PCB_SIZE);

  PICLANG_init();

  return 0;

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

/**
 * Returns the next byte of the program. 
 *
 * Obtains the next byte and then increments the program counter.
 *
 * If the program counter exceeds the program memory, 0xff is returned
 * AND the process status is set to PICLANG_PC_OVERFLOW. Therefore, to determine
 * and error, the status must be checked, not the return value.
 */
char PICLANG_get_next_byte()
{
  unsigned int next = curr_process_addr + curr_process.pc;
  char val;
  if(curr_process.pc > curr_process.size - PCB_SIZE || next < curr_process_addr)
    {
      PICLANG_error(PICLANG_PC_OVERFLOW);
      return 0xff;
    }
  curr_process.pc++;
  next += curr_process.start_address;
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

void PICLANG_update_arith_status()
{
  if(curr_process.stack[curr_process.stack_head - 1] == 0)
    curr_process.bitmap |= PICLANG_ZERO;
  else
    curr_process.bitmap &= ~PICLANG_ZERO;
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
      break;
    case PICLANG_ADD:
      PICLANG_pushl(PICLANG_pop() + PICLANG_pop());
      PICLANG_update_arith_status();
      break;
    case PICLANG_SUB:
      {
	char rhs = PICLANG_pop();
	PICLANG_pushl(PICLANG_pop() - rhs);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_MULT:
      PICLANG_pushl(PICLANG_pop() * PICLANG_pop());
      PICLANG_update_arith_status();
      break;
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
    case PICLANG_FPUTCH:
      {
	picfs_buffer[PICLANG_file_buffer_index++] = PICLANG_pop();
	if(PICLANG_file_buffer_index < FS_BUFFER_SIZE)
	  break;
      }
    case PICLANG_FFLUSH:
      {
	picfs_write(0);
	memset(picfs_buffer,0,FS_BUFFER_SIZE);
	PICLANG_file_buffer_index = 0;
	break;
      }
    case PICLANG_PRINTL:
      IO_putd(PICLANG_pop());
      IO_flush();
      break;
    case PICLANG_CLEAR:
      clear_output();
      break;
    case PICLANG_GETCH:
      {
	PICLANG_pushl(getch());
	break;
      }
    case PICLANG_GETD:
      {
	char dec = getch();
	if(dec < '0' || dec > '9')
	  {
	    PICLANG_error(ARG_INVALID);
	    break;
	  }
	dec -= 0x30;
	PICLANG_pushl(dec);
	break;
      }
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
	addr = PICLANG_pop();
	two[1] = PICLANG_pop();//char or string?
	if(two[1] == PICLANG_MORSE_STRING)
	  SRAM_read(addr++,two,1);
	else
	  {
	    two[0] = (char)addr;
	    two[1] = 0;
	    morse_sound(two);
	    break;
	  }
	two[1] = 0;
	while(two[0] != 0)
	  {
	    morse_sound(two);
	    SRAM_read(addr++,two,1);
	  }
	break;
      }
    case PICLANG_TIME:
      {
	TIME_t *thetime = TIME_get();;
	char key = PICLANG_pop();
	switch(key)
	  {
	  case 0:
	    PICLANG_pushl(thetime->month);
	    break;
	  case 1:
	    PICLANG_pushl(thetime->day);
	    break;
	  case 2:
	    PICLANG_pushl(thetime->hours);
	    break;
	  case 3:
	    PICLANG_pushl(thetime->minutes);
	    break;
	  case 4:
	    PICLANG_pushl(thetime->seconds);
	    break;
	  default:
	    break;
	  }
	break;
      }
      break;
    case PICLANG_SET_TIME:case PICLANG_SET_DATE:
      {
	TIME_t *newtime = TIME_get();
	if(newtime == NULL)
	  {
	    PICLANG_error(PICLANG_NULL_POINTER);
	    break;
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
	    PICLANG_error(error_code);
	    break;
	  }
	PICLANG_pushl((char)argd);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_ARGCH:
      {
	signed char argch = ARG_getch();
	if(argch < 0)
	  {
	    PICLANG_error(error_code);
	    break;
	  }
	PICLANG_pushl((char)argch);
	break;
      }
    case PICLANG_JZ:case PICLANG_JMP:
      {
	char label = PICLANG_get_next_byte();
	if(curr_process.status != PICLANG_SUCCESS)
	  break;
	if(command == PICLANG_JMP)
	  curr_process.pc = label;
	else if((curr_process.bitmap & PICLANG_ZERO) == 0)
	  curr_process.pc = label;
	break;
      }
    case PICLANG_LABEL:
      break;
    case PICLANG_COMPLT:
      {
	char rhs = PICLANG_pop();
	char lhs = PICLANG_pop();
	if(lhs < rhs)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	break;
      }
    case PICLANG_COMPGT:
      {
	char rhs = PICLANG_pop();
	char lhs = PICLANG_pop();
	if(lhs > rhs)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	break;
      }
    case PICLANG_COMPEQ:case PICLANG_COMPNE:
      {
	char rhs = PICLANG_pop();
	char lhs = PICLANG_pop();
	if(lhs == rhs)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	if(command == PICLANG_COMPNE)
	  curr_process.bitmap ^= PICLANG_ZERO;
	break;
      }
    case PICLANG_NUM_COMMANDS:default:
      PICLANG_error(PICLANG_UNKNOWN_COMMAND);
      break;
    }

}
