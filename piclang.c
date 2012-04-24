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
#include <stdbool.h>

// PICLANG_exception sets the status of the program. This will cause the
// program to end.
#define PICLANG_exception(code)  curr_process.status = code

char PICLANG_file_buffer_index;
bit PICLANG_debug = false;
PCB curr_process;

picos_error_t PICLANG_get_errno();
void PICLANG_set_errno();

char PICLANG_load(file_handle_t fh)
{
  thread_id_t new_thread;
  file_t file;
  mount_t mount;
  picos_size_t block_counter;
  extern char ARG_buffer[ARG_SIZE];
  // Check to see if there is an available thread space
  new_thread = thread_allocate();
  picos_processes[new_thread].addr = SRAM_PICFS_FILE_ADDR + PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t);

  // setup arguments
  
  if(ARG_next > 1)
    {
      if(ARG_buffer[ARG_next-1] == 0)
	ARG_buffer[ARG_next-1] = ' ';
    }
  picos_processes[new_thread].nargs = ARG_count();
  ARG_next = 0;
  SRAM_write(SRAM_PICFS_ARG_SWAP_ADDR + ARG_SIZE*new_thread,ARG_buffer,ARG_SIZE);

  // setup file and cat the first block to SRAM
  picos_processes[new_thread].program_file = fh;
  ftab_read(fh*sizeof(file_t)+SRAM_PICFS_FTAB_ADDR,&file,sizeof(file_t));
  SRAM_read(file.mount_point*sizeof(mount_t)+SRAM_MTAB_ADDR,&mount,sizeof(mount_t));

  // Load PCB
  picos_processes[new_thread].block_size = mount.block_size;
  block_counter = 0;
  while(block_counter < PCB_SIZE)
    {    
      picfs_load(fh);
      SRAM_write(SRAM_PICFS_FILE_ADDR + block_counter,(void*)picfs_buffer,mount.block_size);
      block_counter += mount.block_size;
    }

  // Reset current page index 
  picos_processes[new_thread].current_page = -1;
  
  return PICLANG_resume(new_thread);
}

char PICLANG_resume(thread_id_t new_thread)
{
  char size = 0,pos = 0,counter = 0;
  process_addr_t sram_addr;
  char magic_numbers[PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t)];
  extern char ARG_buffer[ARG_SIZE];

  if(new_thread < 0)
    return error_code;
  if(new_thread >= PICOS_MAX_PROCESSES)
    {
      PICLANG_exception(THREAD_TOO_MANY_THREADS);
      return THREAD_TOO_MANY_THREADS;
    }
  sram_addr = picos_processes[new_thread].addr - PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t);
  
  if(sram_addr == PICOS_END_OF_THREADS)
    return PICLANG_NO_SUCH_PROGRAM;

  // Verify this is an executable with the magic number
  SRAM_read(sram_addr,magic_numbers,PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t));
  if(strncmp(magic_numbers,(char*)PICLANG_magic_numbers,PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t)) != 0)
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
      char retval = PAGE_request(curr_process.num_pages,new_thread);
      PICLANG_file_buffer_index = 0;
      if(retval != 0)
	return error_code;
    }
  
  thread_resume(new_thread);
  curr_process.status = PICLANG_SUCCESS;
  picos_processes[new_thread].expires = DEFAULT_PICLANG_QUANTUM;
  picos_processes[new_thread].current_page = -1;// ensures get_next_word will reload the page to SRAM
    
  // reinstate arguments
  SRAM_read(SRAM_PICFS_ARG_SWAP_ADDR + ARG_SIZE*new_thread,ARG_buffer,ARG_SIZE);
  ARG_next = 0;

  return PICLANG_SUCCESS;

}

char PICLANG_save(char saved_status)
{
  if(curr_process.page_size == 0)
    return PICLANG_NO_SUCH_PROGRAM;

  curr_process.status = saved_status;
  SRAM_write(picos_processes[picos_curr_process].addr,&curr_process,PCB_SIZE);
  
  if(saved_status == PICLANG_SUSPENDED)
    thread_suspend(picos_curr_process);
  else
    {
      PAGE_free(picos_curr_process);
      thread_free(picos_curr_process);
    }

  PICLANG_init();

  return 0;

}


void PICLANG_init()
{
  curr_process.page_size = 0;
  curr_process.pc = 0;
  curr_process.status = 0;
  curr_process.start_address = 0;
  curr_process.string_address = 0;
  curr_process.stack_head = 0;
  curr_process.bitmap = 0;
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
  picos_size_t next;
  picos_size_t val;
    
  if(curr_process.pc >= curr_process.string_address*curr_process.page_size/sizeof(picos_size_t))
    {
      PICLANG_exception(PICLANG_PC_OVERFLOW);
      return -1;
    }

  next = FS_BUFFER_SIZE + picos_processes[picos_curr_process].addr;

  if(picos_processes[picos_curr_process].current_page != (curr_process.pc*sizeof(picos_size_t))/curr_process.page_size + curr_process.start_address)
    {
      picos_processes[picos_curr_process].current_page = curr_process.pc*sizeof(picos_size_t)/curr_process.page_size + curr_process.start_address;
      picfs_select_block(picos_processes[picos_curr_process].program_file, picos_processes[picos_curr_process].current_page);
      SRAM_write(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// swap this out
      picfs_load(picos_processes[picos_curr_process].program_file);
      SRAM_write(next,(void*)picfs_buffer,FS_BUFFER_SIZE);
      SRAM_read(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// swap this in
    }
  
  next += (curr_process.pc*sizeof(picos_size_t) % curr_process.page_size);
  
  curr_process.pc++;
  SRAM_read(next,&val,1*sizeof(picos_size_t));
  return val;
}

void PICLANG_pushl(picos_size_t val)
{
  if(curr_process.stack_head >= PICLANG_STACK_SIZE)
    {
      PICLANG_exception(PICLANG_SEGV);
      return;
    }
  curr_process.stack[curr_process.stack_head++] = val;
}

picos_size_t PICLANG_pop()
{
  if(curr_process.stack_head > PICLANG_STACK_SIZE || curr_process.stack_head == 0)
    {
      PICLANG_exception(PICLANG_SEGV);
      return 0;
    }
  return curr_process.stack[--curr_process.stack_head];
}

void PICLANG_call_push(picos_size_t val)
{
  if(curr_process.call_stack_head >= PICLANG_CALL_STACK_SIZE)
    {
      PICLANG_exception(PICLANG_STACK_OVERFLOW);
      return;
    }
  curr_process.call_stack[curr_process.call_stack_head++] = val;
}

picos_size_t PICLANG_call_pop()
{
  if(curr_process.call_stack_head > PICLANG_CALL_STACK_SIZE || curr_process.call_stack_head == 0)
    {
      PICLANG_exception(PICLANG_STACK_OVERFLOW);
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

void PICLANG_block()
{
  curr_process.bitmap |= PICLANG_BLOCKING_CALL;
}

void PICLANG_unblock()
{
  curr_process.bitmap &= ~PICLANG_BLOCKING_CALL; 
}

signed char PICLANG_next_dereference(picos_size_t a/*ddress*/, picos_size_t command)
{
  extern char ARG_buffer[ARG_SIZE];
  signed char retval = SUCCESS;
  const char *array = ARG_buffer;
  SRAM_write(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// swap this out
  if(a >= ARG_SIZE)
    {
      a -= ARG_SIZE;
      if(a >= picos_processes[picos_curr_process].block_size)
	{
	  a -= picos_processes[picos_curr_process].block_size;
	  picfs_select_block(picos_processes[picos_curr_process].program_file,curr_process.string_address + (a/picos_processes[picos_curr_process].block_size));
	  picfs_load(picos_processes[picos_curr_process].program_file);
	}
      array = (const char*)picfs_buffer;
    }

  switch(command)
    {
    case PICLANG_DEREF:
      PICLANG_pushl(array[a]);
      break;
    case PICLANG_SPRINT:
      IO_puts((const char*)(array+a));
      break;
    case PICLANG_SPRINTN:
      {
	picos_size_t idx = 0;
	picos_size_t nchars = PICLANG_pop();
	for(;idx < nchars;idx++)
	  putch(array[a+idx]);
	break;
      }
    case PICLANG_FOPEN:
      retval = picfs_open((const char*)(array+a),curr_dir);
    default:
      break;
    }
	
  SRAM_read(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);// swap this back in
  
  return retval;
}

void PICLANG_next()
{
  picos_size_t command;
  picos_size_t a,b,c;// parameters, can be shared
  picos_signed_t siga;// a, but signed.
  signed char sch1;// signed char parameters
  char ch1;// char parameter
  static bit flag1;

  extern char ARG_buffer[ARG_SIZE];

  if(curr_process.page_size == 0)
    return;
  
  if(picos_processes[picos_curr_process].expires == 0)
    {
      if((curr_process.bitmap & PICLANG_BLOCKING_CALL) == 0 || PICLANG_debug == true)// Check for blocking call
	{
	  PICLANG_save(PICLANG_SUSPENDED);
	  return;
	}
    }
  
  if(picos_processes[picos_curr_process].signal_sent != PICOS_NUM_SIGNALS)
    {
      if(signal_valid_id(picos_processes[picos_curr_process].signal_sent) == false)
	{
	  PICLANG_exception(THREAD_INVALID_SIGNAL);
	  return;
	}
      curr_process.pc = signals[picos_processes[picos_curr_process].signal_sent].sig_action_addr;
      signal_free(picos_processes[picos_curr_process].signal_sent);
      picos_processes[picos_curr_process].signal_sent = PICOS_NUM_SIGNALS;
    }

  command = PICLANG_get_next_word();
  if(curr_process.status != PICLANG_SUCCESS)
    return;

  if(PICLANG_debug == true)
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
	b = PAGE_get(a,picos_curr_process);
	if(error_code != SUCCESS)
	  PICLANG_exception(error_code);
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
	PAGE_set(a,b,picos_curr_process);
	break;
      }
    case PICLANG_ARGC:
      PICLANG_pushl(picos_processes[picos_curr_process].nargs);
      break;
    case PICLANG_ARGV:
      {
	a = PICLANG_pop();
	sch1 = ARG_get(a);
	if(sch1 < 0)
	  {
	    PICLANG_exception(PICLANG_INVALID_PARAMETER);
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
	PICLANG_next_dereference(b+a,command);
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
	flag1 = false;
	for(;ch1 < 5;ch1++)
	  {
	    if(flag1 == true || hex_val[ch1] != 0x30)
	      {
		SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER+PICLANG_file_buffer_index++,&hex_val[ch1],sizeof(char));
		flag1 = true;
	      }
	    if(PICLANG_file_buffer_index >= FS_BUFFER_SIZE)
	      {
		picfs_dump(picos_processes[picos_curr_process].program_file);
		SRAM_write(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
		memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
		SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER,(void*)picfs_buffer,FS_BUFFER_SIZE);
		SRAM_read(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
		PICLANG_file_buffer_index = 0;
	      }
	  }
	if(flag1 == false)
	  {
	    ch1 = '0';
	    SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER+PICLANG_file_buffer_index++,&ch1,sizeof(char));
	    if(PICLANG_file_buffer_index >= FS_BUFFER_SIZE)
	      {
		picfs_dump(picos_processes[picos_curr_process].program_file);
		SRAM_write(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
		memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
		SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER,(void*)picfs_buffer,FS_BUFFER_SIZE);
		SRAM_read(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
		PICLANG_file_buffer_index = 0;
	      }
	  }
	break;
      }
    case PICLANG_PWDIR:
      PICLANG_pushl(curr_dir);
      break;
    case PICLANG_MOUNT:
      a = PICLANG_pop();// device
      b = PICLANG_pop();// address of first byte
      if(picfs_mount((picos_addr_t)b,(picos_dev_t)a) != SUCCESS)
	curr_process.status = error_code;
      break;
    case PICLANG_CHDIR:
      {
	a = PICLANG_pop();
	picfs_chdir(a);
	break;
      }
    case PICLANG_MOVE:
      a = PICLANG_pop();// y
      b = PICLANG_pop();// x
      IO_move(a,b);
      break;
    case PICLANG_GETY: case PICLANG_GETX:
      IO_getxy(&a,&b);
      if(command == PICLANG_GETX)
	PICLANG_pushl(a);
      else
	PICLANG_pushl(b);
      break;
    case PICLANG_FPUTCH:// KEEP FPUTCH before FFLUSH
      {
	ch1 = (char)PICLANG_pop();
	SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER+PICLANG_file_buffer_index++,&ch1,sizeof(char));
	if(PICLANG_file_buffer_index < FS_BUFFER_SIZE)
	  break;
      }
    case PICLANG_FFLUSH:// KEEP FPUTCH before FFLUSH  KEEP FFLUSH before FCLEAR
      picfs_dump(picos_processes[picos_curr_process].program_file);
    case PICLANG_FCLEAR:// KEEP FFLUSH before FCLEAR
      SRAM_write(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
      memset((char*)picfs_buffer,0,FS_BUFFER_SIZE);
      SRAM_write(SRAM_PICLANG_RAW_FILE_BUFFER,(void*)picfs_buffer,FS_BUFFER_SIZE);
      SRAM_read(SRAM_PICLANG_NEXT_SWAP_ADDR,(void*)picfs_buffer,FS_BUFFER_SIZE);
      PICLANG_file_buffer_index = 0;
      break;
    case PICLANG_FSTAT:
      a = PICLANG_pop();
      sch1 = picfs_stat(a);
      if(sch1 < 0)
	{
	  PICLANG_set_errno();
	  sch1 = 0;
	}
      a = picfs_buffer[ST_SIZE] << 8;
      a += picfs_buffer[ST_SIZE+1];
      PICLANG_pushl(a);
      break;
    case PICLANG_READDIR:
      a = PICLANG_pop();
      sch1 = picfs_readdir(1,a);
      if(sch1 != SUCCESS)
	PICLANG_set_errno();
      PICLANG_pushl(sch1);
      break;
    case PICLANG_FOPEN:
      {
	/*	mount_t mount;
		SRAM_read(curr_dir*sizeof(mount_t)+SRAM_MTAB_ADDR,&mount,sizeof(mount_t));*/
	a = PICLANG_pop();
	if(a < ARG_SIZE)
	  {
	    sch1 = picfs_open(ARG_buffer+a,curr_dir);
	  }
	else if(a < ARG_SIZE + FS_BUFFER_SIZE)
	  {
	    a -= ARG_SIZE;
	    sch1 = picfs_open((const char*)picfs_buffer+a,curr_dir);
	  }
	else
	  {
	    c = 0;
	    a -= ARG_SIZE + picos_processes[picos_curr_process].block_size;
	    sch1 = PICLANG_next_dereference(a,command);
	  }
	if(sch1 == -1)
	  PICLANG_set_errno();
	PICLANG_pushl(sch1);
	break;
      }
    case PICLANG_FCLOSE:
      a = PICLANG_pop();
      picfs_close(a);
      break;
    case PICLANG_FREAD:
      a = PICLANG_pop();
      sch1 = picfs_load(a);
      b = sch1;
      if(sch1 < 0)
	{
	  b = -1;
	  error_code = SUCCESS;
	  PICLANG_exception(error_code);
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
    case PICLANG_MUTEX_LOCK:
      PICLANG_block();
      break;
    case PICLANG_MUTEX_UNLOCK:
      PICLANG_unblock();
      break;
    case PICLANG_GETCH:
      {
	PICLANG_block();
	PICLANG_pushl(getch());
	PICLANG_unblock();
	break;
      }
    case PICLANG_GETD:
      {
	PICLANG_block();
	ch1 = getch();
	PICLANG_unblock();
	if(ch1 < '0' || ch1 > '9')
	  {
	    PICLANG_exception(ARG_INVALID);
	    break;
	  }
	ch1 -= 0x30;
	PICLANG_pushl(ch1);
	break;
      }
    case PICLANG_SPRINT:case PICLANG_SPRINTN:
      {
	// string addresses start with arguments then const strings in executable
	/*string_pointer*/a = PICLANG_pop();
	PICLANG_next_dereference(a,command);
	IO_flush();
	break;
      }
    case PICLANG_LSOF:
      IO_puts("Implement lsof");
      break;
    case PICLANG_LSMOUNT:
      lsmount();
      break;
    case PICLANG_ERRNO:
      PICLANG_pushl(PICLANG_get_errno());
      break;
    case PICLANG_SIGNAL:
      a = PICLANG_get_next_word();// signal id
      b = PICLANG_get_next_word();// signal action
      if(a == PICOS_NUM_SIGNALS)
	{
	  signal_free(b);
	  break;
	}
      sch1 = signal_assign(a,picos_curr_process,b);
      if(sch1 != 0)
	PICLANG_exception(error_code);
      break;
    case PICLANG_SLEEP:
      a = PICLANG_pop();
      ch1 = picos_curr_process;
      PICLANG_save(PICLANG_SUSPENDED);
      picos_processes[ch1].expires = (quantum_t)a;
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
	    PICLANG_exception(PICLANG_INVALID_PARAMETER);
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
		PICLANG_exception(TIME_INVALID);
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
		PICLANG_exception(TIME_INVALID);
	      }
	  }
	TIME_set(&newtime);
	break;
      }
    case PICLANG_JZ:case PICLANG_JMP:case PICLANG_CALL:
      {
	b = curr_process.pc;
	a = PICLANG_get_next_word();
	if(curr_process.status != PICLANG_SUCCESS)
	  {
	    curr_process.pc = b;
	    break;
	  }
	if(command == PICLANG_CALL)
	  {
	    PICLANG_call_push(curr_process.pc);
	    curr_process.pc = a;
	  }
	else if(command == PICLANG_JMP)
	  curr_process.pc = a;
	else if((curr_process.bitmap & PICLANG_ZERO) == 0)
	  curr_process.pc = a;
	if(curr_process.status != PICLANG_SUCCESS)
	  {
	    curr_process.pc = b;
	    break;
	  }
	break;
      }
    case PICLANG_RETURN:
      {
	b = curr_process.pc;
	curr_process.pc = PICLANG_call_pop();
	if(curr_process.status != PICLANG_SUCCESS)
	  {
	    curr_process.pc = b;
	    break;
	  }
	break;
      }
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
    case PICLANG_RAWLOAD:
      SRAM_read(SRAM_PICLANG_RAW_FILE_BUFFER,(void*)picfs_buffer,FS_BUFFER_SIZE);
      break;
    case PICLANG_NUM_COMMANDS:default:
      PICLANG_exception(PICLANG_UNKNOWN_COMMAND);
      break;
    }

}

void PICLANG_set_errno()
{
  picos_processes[picos_curr_process].piclang_errno = error_code;
  error_code = curr_process.status = SUCCESS;
}

picos_error_t PICLANG_get_errno()
{
  if(picos_curr_process == -1 || picos_curr_process >= PICOS_MAX_PROCESSES)
    return SUCCESS;// not running anything
  return picos_processes[picos_curr_process].piclang_errno;
}


