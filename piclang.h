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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define EOP 0xff // end of program

#ifndef DEFAULT_PICLANG_QUANTUM
#define DEFAULT_PICLANG_QUANTUM 25
#endif

#ifndef PICLANG_STACK_SIZE
#define PICLANG_STACK_SIZE 0x10
#endif

#ifndef PICLANG_CALL_STACK_SIZE
#define PICLANG_CALL_STACK_SIZE 0x8
#endif

// bitmap masks
#define PICLANG_ZERO 0x1 // Set if the result of arithmetic is zero OR if a boolen arithmetic is TRUE (cleared if FALSE)
typedef unsigned short picos_size_t;// On the picc, this is little endian!!!

typedef struct{
  picos_size_t size;// size of program
  char bitmap;// See bitmap mask defines above
  char num_pages;
  picos_size_t pc;// program counter
  char status;// Error status
  picos_size_t start_address;// Offset of SRAM used for program (after PCB)
  picos_size_t string_address;// Offset of string data (null terminated)
  char stack[PICLANG_STACK_SIZE];
  char stack_head;
  char call_stack[PICLANG_CALL_STACK_SIZE];
  char call_stack_head;
}PCB;
#define PCB_SIZE sizeof(PCB)
#define PCB_MAGIC_NUMBER_OFFSET 4
extern const char *PICLANG_magic_numbers;

extern PCB curr_process;
unsigned int curr_process_addr;// Address of first byte of program (including PCB) in memory that is currently running.
volatile char PICLANG_quantum;
extern int PICLANG_next_process[];
volatile char PICLANG_system;// CRC hex value for system calls within piclan

extern void PICLANG_init();

/**
 * Loads program from SRAM.
 * Returns status of load
 */
extern char PICLANG_load(unsigned int sram_addr);

/**
 * Saves the program to SRAM
 * Returns status of program at save.
 */
extern char PICLANG_save(char status);

extern void PICLANG_next();

extern char PICLANG_pop();

extern bit PICLANG_debug;
extern void PICLANG_debug_out(char opcode);//define to do something as a debugger

enum PICLANG_COMMANDS
  {
    PICLANG_ADD=0, 
    PICLANG_SUB, 
    PICLANG_MULT,
    PICLANG_PUSHL,
    PICLANG_PUSH,
    PICLANG_POP,
    PICLANG_PRINT,
    PICLANG_PRINTL,
    PICLANG_CLEAR,
    PICLANG_SPRINT,
    PICLANG_FPUTCH,
    PICLANG_FPUTD,
    PICLANG_FFLUSH,
    PICLANG_SYSTEM,
    PICLANG_MORSE,
    PICLANG_TIME,
    PICLANG_ARGD/* reads a digit from the command line */,
    PICLANG_SET_TIME,
    PICLANG_SET_DATE,
    PICLANG_ARGCH,
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
    PICLANG_FCLEAR/* Clears the picfs buffer */,
    PICLANG_NUM_COMMANDS
  };

#define PICLANG_SUCCESS 0

enum PICLANG_MORSE_TYPES{PICLANG_MORSE_CHAR,PICLANG_MORSE_STRING};


#endif //PICLANG_H
