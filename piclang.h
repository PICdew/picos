/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file gives structures and function prototypes 
 * for the PICLANG programming language.
 */
#ifndef PICLANG_H
#define PICLANG_H 1

#include "utils.h"
#include "scheduler.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef DEFAULT_PICLANG_QUANTUM
#define DEFAULT_PICLANG_QUANTUM 25
#endif

#ifndef PICLANG_STACK_SIZE
#define PICLANG_STACK_SIZE 0x10
#endif

#ifndef PICLANG_CALL_STACK_SIZE
#define PICLANG_CALL_STACK_SIZE 0x8
#endif

#define PICLANG_STRING_OFFSET (ARG_SIZE + FS_BUFFER_SIZE)

// bitmap masks
#define PICLANG_ZERO 0x1 // Set if the result of arithmetic is zero OR if a boolen arithmetic is TRUE (cleared if FALSE)
#define PICLANG_BLOCKING_CALL 0x2// Set if the program should NOT loose control of IO

typedef struct{
  picos_size_t page_size;// Size of data pages
  picos_size_t bitmap;// See bitmap mask defines above
  picos_size_t num_pages;// Number of RAM pages
  picos_size_t pc;// program counter
  picos_size_t status;// Error status
  picos_size_t start_address;// Index of first page of data (zero-index)
  picos_size_t string_address;// Index of ifrst page of string text (zero-index)
  picos_size_t stack[PICLANG_STACK_SIZE];
  picos_size_t stack_head;
  picos_size_t call_stack[PICLANG_CALL_STACK_SIZE];
  picos_size_t call_stack_head;
}PCB;
#define PCB_SIZE sizeof(PCB)
#define PCB_MAGIC_NUMBER_OFFSET 2 // 2 x picos_size_t 
extern const picos_size_t PICLANG_magic_numbers[PCB_MAGIC_NUMBER_OFFSET];

extern bit PICLANG_debug;
extern PCB curr_process;
volatile picos_size_t PICLANG_system;// CRC hex value for system calls within piclan

extern void PICLANG_init();

/**
 * Loads program from SRAM.
 * Returns status of load
 */
extern char PICLANG_load(file_handle_t file);

extern char PICLANG_resume(thread_id_t new_thread);

/**
 * Saves the program to SRAM
 * Returns status of program at save.
 */
extern char PICLANG_save(char status);

extern void PICLANG_next();

extern picos_size_t PICLANG_pop();

extern void PICLANG_debug_out(picos_size_t opcode);//define to do something as a debugger

enum PICLANG_COMMANDS
  {
    PICLANG_ADD=0, 
    PICLANG_SUB, 
    PICLANG_MULT,
    PICLANG_DIV/* INTEGER quotient */,
    PICLANG_MOD/* Modulus operator */,
    PICLANG_BSL/* Bit shift left */,
    PICLANG_BSR/* Bit shift right */,
    PICLANG_AND/* Bitwise and */,
    PICLANG_OR/* Bitwise or */,
    PICLANG_NOT/* Bitwise complement */,
    PICLANG_UMINUS/* Unary minus, additive inverse */,
    PICLANG_PUSHL/* Push value as a literal number */,
    PICLANG_PUSH/* interpret as a decimal and push */,
    PICLANG_POP/* save the top of the stack to memory*/,
    PICLANG_DROP/* remove the top of the stack without saving it to memory */,
    PICLANG_SWAP/* swap top two entries of stack */,
    PICLANG_PRINT,
    PICLANG_PRINTL,
    PICLANG_CLEAR,
    PICLANG_SPRINT,
    PICLANG_FPUTCH,
    PICLANG_FPUTD,
    PICLANG_FFLUSH,
    PICLANG_FCLEAR/* Clears the picfs buffer */,
    PICLANG_FOPEN,
    PICLANG_FCLOSE,
    PICLANG_FREAD,
    PICLANG_SYSTEM,
    PICLANG_SIGNAL/* set a signal */,
    PICLANG_SLEEP,
    PICLANG_MORSE,
    PICLANG_TIME,
    PICLANG_SET_TIME,
    PICLANG_SET_DATE,
    PICLANG_ARGC/* Number of arguments including command */,
    PICLANG_ARGV/* Get the index of the first character of the specified command argument*/,
    PICLANG_GETCH/* retrieve a character from stdin */,
    PICLANG_GETD/* retrieve a decimial integer from 0 to 9 from stdin */,
    PICLANG_JZ/* jump if zero */,
    PICLANG_JMP/* jump unconditionally */,
    PICLANG_COMPLT/* test for less than */,
    PICLANG_COMPGT/* test for greather than */,
    PICLANG_COMPEQ/* test for equality */,
    PICLANG_COMPNE/* inequality */,
    PICLANG_LABEL/* Dummy variable for labeling sections of code for jumps */,
    PICLANG_CALL/* Call a subroutine (using the lables) */,
    PICLANG_RETURN/* Return from a subroutine */,
    PICLANG_EXIT/* Exit a program */,
    PICLANG_DEREF/* dereference operation */,
    PICLANG_CHDIR/* Change working directory */,
    PICLANG_PWDIR/* Puts working directory on stack*/,
    PICLANG_MOUNT/* Mount a directory */,
    PICLANG_MUTEX_LOCK/* Lock thread */,
    PICLANG_MUTEX_UNLOCK/* Unlock thread*/,
    PICLANG_NUM_COMMANDS
  };

#define PICLANG_SUCCESS 0

enum PICLANG_MORSE_TYPES{PICLANG_MORSE_CHAR,PICLANG_MORSE_STRING};


#endif //PICLANG_H
