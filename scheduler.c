#include "scheduler.h"
#include "picfs.h"
#include "picfs_error.h"

#include <stdbool.h>

picos_thread picos_processes[PICOS_MAX_PROCESSES];
thread_id_t picos_free_queue,picos_wait_queue, picos_curr_process;

char thread_valid_id(thread_id_t tid)
{
  if(tid > -1 && tid < PICOS_MAX_PROCESSES)
    return true;
  return false;
}

char signal_valid_id(picos_signal_t sid)
{
  if(sid < PICOS_NUM_SIGNALS)
    return true;
  return false;
}

void thread_init()
{
  char i = 0;
  picos_wait_queue = -1;
  picos_curr_process = -1;
  picos_free_queue = 0;
  for(;i<PICOS_MAX_PROCESSES;i++)
    {
      picos_processes[i].addr = PICOS_END_OF_THREADS;
      picos_processes[i].expires = 0;
      picos_processes[i].nargs = 0;
      picos_processes[i].signal_sent = PICOS_NUM_SIGNALS;
      picos_processes[i].next = i+1;
      picos_processes[i].previous = i-1;
    }
  picos_processes[PICOS_MAX_PROCESSES-1].next = -1;
}

thread_id_t thread_allocate()
{
  thread_id_t retval = -1;
  if(picos_free_queue < 0)
    return error_return(THREAD_TOO_MANY_THREADS);
  
  retval = picos_free_queue;
  picos_free_queue = picos_processes[retval].next;
  picos_processes[picos_free_queue].previous = -1;
  
  picos_processes[retval].block_size = 0;
  picos_processes[retval].program_file = 0xff;
  picos_processes[retval].addr = PICOS_END_OF_THREADS;
  picos_processes[retval].expires = 0;
  picos_processes[retval].signal_sent = PICOS_NUM_SIGNALS;
  picos_processes[retval].next = picos_wait_queue;
  if(thread_valid_id(picos_wait_queue))
    picos_processes[picos_wait_queue].previous = retval;
  
  picos_wait_queue = retval;
  return retval;
}

signed char thread_suspend(quantum_t suspend_priority)
{
  thread_id_t previous;
  if(picos_curr_process < 0 || picos_curr_process >= PICOS_MAX_PROCESSES)
    return 0;
  
  if(picos_wait_queue < 0)
    {
      picos_processes[picos_curr_process].next = -1;
      picos_processes[picos_curr_process].previous = -1;
      picos_processes[picos_curr_process].expires = suspend_priority;
      picos_wait_queue = picos_curr_process;
      picos_curr_process = -1;
      return 0;
    }
  
  previous = picos_wait_queue;
  while(picos_processes[previous].next != -1)
    {
      if(picos_processes[picos_processes[previous].next].expires > suspend_priority)
	break;
      previous = picos_processes[previous].next;
    }
  picos_processes[picos_curr_process].next = picos_processes[previous].next;
  picos_processes[picos_curr_process].previous = previous;
  picos_processes[previous].next = picos_curr_process;
  picos_processes[picos_processes[picos_curr_process].next].previous = picos_curr_process;
  picos_curr_process = -1;
  
  return 0;  
      
}

signed char thread_resume(thread_id_t new_thread)
{
  if(picos_curr_process > -1)
    return error_return(THREAD_CANNOT_INTERRUPT);
  
  if(thread_valid_id(new_thread) == false)
    return error_return(THREAD_INVALID_SIGNAL);

  if(picos_processes[new_thread].next > -1)
    picos_processes[picos_processes[new_thread].next].previous = picos_processes[new_thread].previous;
  if(picos_processes[new_thread].previous > -1)
    picos_processes[picos_processes[new_thread].previous].next = picos_processes[new_thread].next;
  
  if(new_thread == picos_wait_queue)
    picos_wait_queue = picos_processes[new_thread].next;
  
  picos_processes[new_thread].next = -1;
  picos_processes[new_thread].previous = -1;
  
  picos_curr_process = new_thread;
  return 0;
}

void thread_free(thread_id_t tid)
{
  picos_signal_t sigs;
  if(thread_valid_id(tid) == false)
    return;

  if(picos_processes[tid].next > -1)
    picos_processes[picos_processes[tid].next].previous = picos_processes[tid].previous;
  if(picos_processes[tid].previous > -1)
    picos_processes[picos_processes[tid].previous].next = picos_processes[tid].next;

  if(picfs_close(picos_processes[tid].program_file))
    error_code = SUCCESS;
  
  picos_processes[tid].addr = PICOS_END_OF_THREADS;
  picos_processes[tid].expires = 0;
  picos_processes[tid].nargs = 0;
  picos_processes[tid].signal_sent = PICOS_NUM_SIGNALS;
  picos_processes[tid].next = picos_free_queue;
  picos_processes[picos_free_queue].previous = tid;
  picos_processes[tid].previous = -1;
  
  picos_free_queue = tid;
  picos_curr_process = -1;

  sigs = 0;
  for(;sigs < PICOS_NUM_SIGNALS;sigs++)
    {
      if(signals[sigs].owner == tid)
	signal_free(sigs);
    }
  
}

signed char signal_assign(picos_signal_t signal, thread_id_t thread, picos_size_t handler_addr)
{
  if(signal_valid_id(signal) == false)
    return error_return(THREAD_INVALID_SIGNAL);
  if(signals[signal].owner != PICOS_MAX_PROCESSES)
    return error_return(PICFS_EAGAIN);

  signals[signal].owner = thread;
  signals[signal].sig_action_addr = handler_addr;
  return 0;
}


signed char signal_send(picos_signal_t signal)
{
  if(signal_valid_id(signal) == false)
    return error_return(THREAD_INVALID_SIGNAL);
  if(signals[signal].owner == PICOS_MAX_PROCESSES)
    return 0;
  
  if(thread_valid_id(signals[signal].owner) == false)
    {
      signal_free(signal);
      return 0;
    }

  picos_processes[signals[signal].owner].signal_sent = signal;
  return 0;
}

void signal_free(picos_signal_t signal)
{
  signals[signal].owner = PICOS_MAX_PROCESSES;
  signals[signal].sig_action_addr = 0;
}

void signal_init()
{
  picos_signal_t sigs = 0;
  for(;sigs < PICOS_NUM_SIGNALS;sigs++)
    signal_free(sigs);
}


