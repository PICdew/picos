#ifndef SCHEDULER_H
#define SCHEDULER_H 1


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "picos_time.h"
#include "utils.h"

#ifndef PICOS_MAX_PROCESSES 
#define PICOS_MAX_PROCESSES 4
#endif

#define PICOS_END_OF_THREADS 0xffff

typedef unsigned int process_addr_t;
typedef signed char thread_id_t;
typedef unsigned char picos_signal_t;

typedef struct {
  process_addr_t addr;// starting address in memory
  quantum_t expires;// system clock ticks left in the threads state
  char nargs;// number of args
  file_handle_t program_file;
  char block_size;// block size of FS in which the file is stored
  picos_size_t current_page;// Index of the page of the program currently loaded.
  picos_signal_t signal_sent;// value of the last sent signal
  thread_id_t next;// next thread in queue
  thread_id_t previous;// previous thread in queue
}picos_thread;

extern picos_thread picos_processes[PICOS_MAX_PROCESSES];
extern thread_id_t picos_wait_queue;// head of queue of processes waiting to be run
extern thread_id_t picos_curr_process;// process running

// Thread functions
void thread_init();
thread_id_t thread_allocate();
signed char thread_suspend(quantum_t suspend_priority);// suspend the running process. Lower the priority number, the sooner it will be run
signed char thread_resume(thread_id_t new_thread);// switch threads
void thread_free(thread_id_t tid);// faultfree removal of a thread

// Signal functions
signed char signal_assign(picos_signal_t signal, thread_id_t thread, picos_size_t handler_addr);// assigns a signal to a thread, if it is free.
signed char signal_send(picos_signal_t signal);// indicates the use of the signal
void signal_free(picos_signal_t signal);// unassigns a signal
void signal_init();
char signal_valid_id(picos_signal_t sid);

enum PICOS_SIGNAL{
  PICOS_SIGINT = 0/* interrupt key */,
  PICOS_NUM_SIGNALS
};

typedef struct {
  thread_id_t owner;
  picos_size_t sig_action_addr;// PC value to be used when signal is given
}picos_signal_handler;
picos_signal_handler signals[PICOS_NUM_SIGNALS];


#endif
