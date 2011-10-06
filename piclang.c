/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file provides the code for running PICLANG programs.
 */
#include "page.h"
#include "picfs_error.h"
#include "picfs.h"
#include "picos_time.h"
#include "piclang.h"
#include "arg.h"
#include "io.h"
#include "utils.h"
#include "sram.h"

#include <stdio.h>
#include <string.h>

#define PICLANG_error(code)  curr_process.status = code
char PICLANG_file_buffer_index;
bit PICLANG_debug = FALSE;
PCB curr_process;

char PICLANG_load(process_addr_t sram_addr)
{
  thread_id_t new_thread;
  // Check to see if there is an available thread space
  new_thread = thread_allocate();
  picos_processes[new_thread].addr = sram_addr;
  PICLANG_resume(new_thread);
}

char PICLANG_resume(thread_id_t new_thread)
{
  char size = 0,pos = 0,counter = 0;
  process_addr_t sram_addr;
  char magic_numbers[PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t)];

  if(new_thread < 0)
    return error_code;
  if(new_thread >= PICOS_MAX_PROCESSES)
    {
      PICLANG_error(THREAD_TOO_MANY_THREADS);
      return THREAD_TOO_MANY_THREADS;
    }
  sram_addr = picos_processes[new_thread].addr;
  
  if(sram_addr == 0xffff)
    return PICLANG_NO_SUCH_PROGRAM;

  // Verify this is an executable with the magic number
  SRAM_read(sram_addr,magic_numbers,PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t));
  if(strncmp(magic_numbers,PICLANG_magic_numbers,PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t)) != 0)
    {
      error_code = PICLANG_INVALID_EXECUTABLE;
      return error_code;
    }
  sram_addr += PCB_MAGIC_NUMBER_OFFSET * sizeof(picos_size_t);

  picos_processes[new_thread].expires = DEFAULT_PICLANG_QUANTUM;
  picos_processes[new_thread].addr = sram_addr;
  
  // Load PCB into memory
  SRAM_read(sram_addr,&curr_process,PCB_SIZE);

  if(curr_process.status != PICLANG_SUSPENDED)
    {
      char retval = PAGE_request(curr_process.num_pages,0 /* replace with UID or pid */);
      PICLANG_file_buffer_index = 0;
      if(retval != 0)
	return error_code;
    }
  
  thread_resume(new_thread);
    
  picos_processes[new_thread].expires = DEFAULT_PICLANG_QUANTUM;

  return PICLANG_SUCCESS;

}

char PICLANG_save(char saved_status)
{
  if(curr_process.size == 0)
    return PICLANG_NO_SUCH_PROGRAM;

  curr_process.status = saved_status;
  SRAM_write(picos_processes[picos_curr_process].addr,&curr_process,PCB_SIZE);
  
  if(saved_status == PICLANG_SUSPENDED)
    thread_suspend(picos_curr_process);
  else
    {
      PAGE_free(0);
      thread_free(picos_curr_process);
    }

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
picos_size_t PICLANG_get_next_word()
{
  picos_size_t next = picos_processes[picos_curr_process].addr + curr_process.pc*sizeof(picos_size_t);
  picos_size_t val;
  if(curr_process.pc > curr_process.string_address - curr_process.start_address || next < picos_processes[picos_curr_process].addr)
    {
      PICLANG_error(PICLANG_PC_OVERFLOW);
      return 0xff;
    }
  curr_process.pc++;
  next += curr_process.start_address*sizeof(picos_size_t);
  SRAM_read(next,&val,1*sizeof(picos_size_t));
  return val;
}

void PICLANG_pushl(picos_size_t val)
{
  if(curr_process.stack_head >= PICLANG_STACK_SIZE)
    {
      PICLANG_error(PICLANG_SEGV);
      return;
    }
  curr_process.stack[curr_process.stack_head++] = val;
}

picos_size_t PICLANG_pop()
{
  if(curr_process.stack_head > PICLANG_STACK_SIZE || curr_process.stack_head == 0)
    {
      PICLANG_error(PICLANG_SEGV);
      return 0;
    }
  return curr_process.stack[--curr_process.stack_head];
}

void PICLANG_call_push(picos_size_t val)
{
  if(curr_process.call_stack_head >= PICLANG_CALL_STACK_SIZE)
    {
      PICLANG_error(PICLANG_STACK_OVERFLOW);
      return;
    }
  curr_process.call_stack[curr_process.call_stack_head++] = val;
}

picos_size_t PICLANG_call_pop()
{
  if(curr_process.call_stack_head > PICLANG_CALL_STACK_SIZE)
    {
      PICLANG_error(PICLANG_STACK_OVERFLOW);
      return 0;
    }
  return curr_process.call_stack[--curr_process.call_stack_head];
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
  picos_size_t command;
  picos_size_t a,b,c;// parameters, can be shared
  signed char sch1;// signed char parameters
  char ch1;// char parameter
  static bit flag1;

  extern char ARG_buffer[ARG_SIZE];

  if(curr_process.size == 0)
    return;
  
  if(picos_processes[picos_curr_process].expires == 0)
    {
      PICLANG_save(PICLANG_SUSPENDED);
      return;
    }
  
  command = PICLANG_get_next_word();
  if(curr_process.status != PICLANG_SUCCESS)
    return;

  if(PICLANG_debug == TRUE)
    PICLANG_debug_out(command);
  
  switch(command)
    {
    case PICLANG_UMINUS:
      {
	picos_signed_t inv = (picos_signed_t)PICLANG_pop();
	PICLANG_pushl(-inv);
	break;
      }
    case PICLANG_ADD:
      PICLANG_pushl(PICLANG_pop() + PICLANG_pop());
      PICLANG_update_arith_status();
      break;
    case PICLANG_MOD:case PICLANG_DIV:
      {
	// a/b --> c rem a
	b = PICLANG_pop();
	a = PICLANG_pop();
	c = 0;
	while(a >= b)
	  {
	    a -= b;
	    c++;
	  }
	if(command == PICLANG_MOD)
	  PICLANG_pushl(a);
	else
	  PICLANG_pushl(c);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_SUB:
      {
	b = PICLANG_pop();
	a = PICLANG_pop();
	PICLANG_pushl(a - b);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_MULT:
      a = PICLANG_pop();
      b = PICLANG_pop();
      c = a*b;
      PICLANG_pushl(c);
      PICLANG_update_arith_status();
      break;
    case PICLANG_PUSHL:
      a = PICLANG_get_next_word();
      PICLANG_pushl(a);
      break;
    case PICLANG_PUSH:
      {
	a = PICLANG_get_next_word();
	b = PAGE_get(a,0/* replace with UID */);
	if(error_code != SUCCESS)
	  PICLANG_error(error_code);
	else
	  PICLANG_pushl(b);
	break;
      }
    case PICLANG_DROP:
      PICLANG_pop();
      break;
    case PICLANG_SWAP:
      a = PICLANG_pop();
      b = PICLANG_pop();
      PICLANG_pushl(a);
      PICLANG_pushl(b);
      break;
    case PICLANG_POP:
      {
	a = PICLANG_get_next_word();
	b = PICLANG_pop();
	PAGE_set(a,b,0/* replace with UID */);
	break;
      }
    case PICLANG_ARGC:
      a = ARG_count();
      PICLANG_pushl(a);
      break;
    case PICLANG_ARGV:
      {
	a = PICLANG_pop();
	sch1 = ARG_get(a);
	if(sch1 < 0)
	  {
	    PICLANG_error(PICLANG_INVALID_PARAMETER);
	    break;
	  }
	b = (picos_size_t)sch1;
	PICLANG_pushl(b);
	break;
      }
    case PICLANG_PRINT:
      {
	a = PICLANG_pop();
	putch(a);
	IO_flush();
	break;
      }
    case PICLANG_DEREF:
      {
	a = PICLANG_pop();// array index
	b = PICLANG_pop();// array starting address
	if(b < ARG_SIZE)
	  {
	    PICLANG_pushl(*(ARG_buffer+a+b));
	    break;
	  }
	else if(b < ARG_SIZE + FS_BUFFER_SIZE)
	  {
	    b -= ARG_SIZE;
	    PICLANG_pushl(*(picfs_buffer+a+b));
	    break;
	  }
	
	// string section
	b -= ARG_SIZE + FS_BUFFER_SIZE;
	a += b + picos_processes[picos_curr_process].addr + curr_process.string_address*sizeof(picos_size_t);// offset for beginning of string location
	SRAM_read(a,&ch1,1);
	c = ch1;
	PICLANG_pushl(c);
	break;
      }
    case PICLANG_BSL: case PICLANG_BSR:// bit shifts
      {
	a /*shift_amount*/ = PICLANG_pop();
	b /*val*/ = PICLANG_pop();
	if(command == PICLANG_BSL)
	  b <<= a;
	else
	  b >>= a;
	PICLANG_pushl(b);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_FPUTD:
      {
	char hex_val[PICOS_SIZE_T_DECIMAL_DIGITS];//ch1 = index
	dec_to_word(hex_val,PICLANG_pop());
	ch1 = 0;
	flag1 = FALSE;
	for(;ch1 < 5;ch1++)
	  {
	    if(flag1 == TRUE || hex_val[ch1] != 0x30)
	      {
		picfs_buffer[PICLANG_file_buffer_index++] = hex_val[ch1];
		flag1 = TRUE;
	      }
	    if(PICLANG_file_buffer_index >= FS_BUFFER_SIZE)
	      {
		picfs_write(0);
		memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
		PICLANG_file_buffer_index = 0;
	      }
	  }
	if(flag1 == FALSE)
	  {
	    picfs_buffer[PICLANG_file_buffer_index++] = '0';
	    if(PICLANG_file_buffer_index >= FS_BUFFER_SIZE)
	      {
		picfs_write(0);
		memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
		PICLANG_file_buffer_index = 0;
	      }
	  }
	break;
      }
    case PICLANG_FPUTCH:// KEEP FPUTCH before FFLUSH
      {
	picfs_buffer[PICLANG_file_buffer_index++] = PICLANG_pop();
	if(PICLANG_file_buffer_index < FS_BUFFER_SIZE)
	  break;
      }
    case PICLANG_FFLUSH:// KEEP FPUTCH before FFLUSH  KEEP FFLUSH before FCLEAR
	picfs_write(0);
    case PICLANG_FCLEAR:// KEEP FFLUSH before FCLEAR
      memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
      PICLANG_file_buffer_index = 0;
      break;
    case PICLANG_FOPEN:
      {
	a = PICLANG_pop();
	if(a < ARG_SIZE)
	  {
	    sch1 = picfs_open(ARG_buffer+a);
	  }
	else if(a < ARG_SIZE + FS_BUFFER_SIZE)
	  {
	    a -= ARG_SIZE;
	    sch1 = picfs_open((const char*)picfs_buffer+a);
	  }
	else
	  {
	    c = 0;
	    a -= ARG_SIZE + FS_BUFFER_SIZE;
	    a += picos_processes[picos_curr_process].addr + curr_process.string_address*sizeof(picos_size_t);// offset for beginning of string location
	    for(;c< PICFS_FILENAME_MAX;a++,c++)
	      {
		SRAM_read(a,(void*)picfs_buffer+c,1);
		if(picfs_buffer[c] == 0)
		  break;
	      }
	    sch1 = picfs_open((const char*)picfs_buffer);
	  }
	PICLANG_pushl(sch1);
	break;
      }
    case PICLANG_FCLOSE:
      a = PICLANG_pop();
      picfs_close(a);
      break;
    case PICLANG_FREAD:
      a = PICLANG_pop();
      sch1 = picfs_read(a);
      b = sch1;
      if(sch1 < 0)
	{
	  if(error_code == PICFS_EOF)
	    {
	      b = -1;
	      error_code = SUCCESS;
	    }
	  else
	    PICLANG_error(error_code);
	}
      PICLANG_pushl(b);
      break;
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
	ch1 = getch();
	if(ch1 < '0' || ch1 > '9')
	  {
	    PICLANG_error(ARG_INVALID);
	    break;
	  }
	ch1 -= 0x30;
	PICLANG_pushl(ch1);
	break;
      }
    case PICLANG_SPRINT:
      {
	// string addresses start with arguments then const strings in executable
	/*string_pointer*/a = PICLANG_pop();
	if(a < ARG_SIZE)
	  {
	    IO_puts(ARG_buffer + a);
	    IO_flush();
	    break;
	  }
	else if(a < ARG_SIZE + FS_BUFFER_SIZE)
	  {
	    a -= ARG_SIZE;
	    IO_puts((const char*)picfs_buffer + a);
	    IO_flush();
	    break;
	  }
	/*addr*/b = picos_processes[picos_curr_process].addr + curr_process.string_address*sizeof(picos_size_t);
	b += a - ARG_SIZE - FS_BUFFER_SIZE;
	SRAM_read(b++,&ch1,1);
	if(ch1 == 0)
	  flag1 = FALSE;
	else
	  flag1 = TRUE;
	while(ch1 != 0)
	  {
	    putch(ch1);
	    SRAM_read(b++,&ch1,1);
	  }
	if(flag1 == TRUE)
	  IO_flush();
	break;
      }
    case PICLANG_SYSTEM:
      PICLANG_system = PICLANG_pop();
      picos_processes[picos_curr_process].expires = 0;// will suspend for system call
      break;
    case PICLANG_MORSE:
      {
	char two[2];
	/*addr*/a = PICLANG_pop();
	two[1] = PICLANG_pop();//char or string?
	if(two[1] == PICLANG_MORSE_STRING)
	  SRAM_read(a++,two,1);
	else
	  {
	    two[0] = (char)a;
	    two[1] = 0;
	    morse_sound(two);
	    break;
	  }
	two[1] = 0;
	while(two[0] != 0)
	  {
	    morse_sound(two);
	    SRAM_read(a++,two,1);
	  }
	break;
      }
    case PICLANG_TIME:
      {
	const TIME_t *thetime = TIME_get();
	ch1 = PICLANG_pop();
	switch(ch1)
	  {
	  case 'Y':
	    PICLANG_pushl(thetime->year);
	    break;
	  case 'm':
	    PICLANG_pushl(thetime->month);
	    break;
	  case 'd':
	    PICLANG_pushl(thetime->day);
	    break;
	  case 'H':
	    PICLANG_pushl(thetime->hours);
	    break;
	  case 'M':
	    PICLANG_pushl(thetime->minutes);
	    break;
	  case 'S':
	    PICLANG_pushl(thetime->seconds);
	    break;
	  default:
	    PICLANG_error(PICLANG_INVALID_PARAMETER);
	    break;
	  }
	break;
      }
    case PICLANG_SET_TIME:case PICLANG_SET_DATE:
      {
	TIME_t newtime = *(TIME_get());
	if(command == PICLANG_SET_TIME)
	  {
	    newtime.minutes = PICLANG_pop();
	    newtime.hours = PICLANG_pop();
	    if(newtime.minutes > 59 || newtime.hours > 23)
	      {
		newtime.minutes = newtime.hours = 0;
		PICLANG_error(TIME_INVALID);
	      }
	  }
	else
	  {
	    newtime.year = PICLANG_pop();
	    newtime.day = PICLANG_pop();
	    newtime.month = PICLANG_pop();
	    if(newtime.month > 12 || newtime.day > 31)
	      {
		newtime.month = newtime.day = 0;
		PICLANG_error(TIME_INVALID);
	      }
	  }
	TIME_set(&newtime);
	break;
      }
    case PICLANG_ARGD:
      {
	sch1 = ARG_getd();
	if(sch1 < 0)
	  {
	    PICLANG_error(error_code);
	    break;
	  }
	PICLANG_pushl((char)sch1);
	PICLANG_update_arith_status();
	break;
      }
    case PICLANG_ARGCH:
      {
	sch1 = ARG_getch();
	if(sch1 < 0)
	  {
	    PICLANG_error(error_code);
	    break;
	  }
	PICLANG_pushl((char)sch1);
	break;
      }
    case PICLANG_JZ:case PICLANG_JMP:case PICLANG_CALL:
      {
	a = PICLANG_get_next_word();
	if(curr_process.status != PICLANG_SUCCESS)
	  break;
	if(command == PICLANG_CALL)
	  {
	    PICLANG_call_push(curr_process.pc);
	    curr_process.pc = a;
	  }
	else if(command == PICLANG_JMP)
	  curr_process.pc = a;
	else if((curr_process.bitmap & PICLANG_ZERO) == 0)
	  curr_process.pc = a;
	break;
      }
    case PICLANG_RETURN:
      curr_process.pc = PICLANG_call_pop();
      break;
    case PICLANG_EXIT:
      PICLANG_save(PICLANG_pop());
      break;
    case PICLANG_LABEL:
      break;
    case PICLANG_COMPLT:
      {
	b = PICLANG_pop();
	a = PICLANG_pop();
	if(a < b)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	break;
      }
    case PICLANG_COMPGT:
      {
	b = PICLANG_pop();
	a = PICLANG_pop();
	if(a > b)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	break;
      }
    case PICLANG_COMPEQ:case PICLANG_COMPNE:
      {
	b = PICLANG_pop();
	a = PICLANG_pop();
	if(a == b)
	  curr_process.bitmap |= PICLANG_ZERO;
	else
	  curr_process.bitmap &= ~PICLANG_ZERO;
	if(command == PICLANG_COMPNE)
	  curr_process.bitmap ^= PICLANG_ZERO;
	break;
      }
    case PICLANG_AND:
      a = PICLANG_pop();
      b = PICLANG_pop();
      PICLANG_pushl(a & b);
      break;
    case PICLANG_OR:
      a = PICLANG_pop();
      b = PICLANG_pop();
      PICLANG_pushl(a | b);
      break;
    case PICLANG_NOT:
      a = PICLANG_pop();
      PICLANG_pushl(~a);
      break;
    case PICLANG_NUM_COMMANDS:default:
      PICLANG_error(PICLANG_UNKNOWN_COMMAND);
      break;
    }

}
