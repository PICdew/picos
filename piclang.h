#ifndef PICLANG_H
#define PICLANG_H 1

#define EOP 0xff // end of program

#ifndef DEFAULT_PICLANG_QUANTUM
#define DEFAULT_PICLANG_QUANTUM 25
#endif

#ifndef PICLANG_STACK_SIZE
#define PICLANG_STACK_SIZE 0x10
#endif

// bitmap masks
#define PICLANG_BIT_SYSCALL 0x1

typedef struct{
  char size;// size of program
  char bitmap;// flags for the OS, i.e. uses system call arguments?
  char num_pages;
  char pc;// program counter
  char status;// Error status
  char start_address;// Offset of SRAM used for program (after PCB)
  char string_address;// Offset of string data (null terminated)
  char stack[PICLANG_STACK_SIZE];
  char stack_head;
}PCB;
#define PCB_SIZE sizeof(PCB)

PCB curr_process;
unsigned int curr_process_addr;// Address of first byte of program (including PCB) in memory that is currently running.
volatile char PICLANG_quantum;
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

enum PICLANG_COMMANDS
  {
    PICLANG_ADD=0, PICLANG_SUB, PICLANG_MULT,PICLANG_PUSHL,PICLANG_PUSH,
    PICLANG_POP, PICLANG_INPUT,PICLANG_PRINT,PICLANG_PRINTL,
    PICLANG_SPRINT,PICLANG_SYSTEM,PICLANG_MORSE,PICLANG_TIME,
    PICLANG_NUM_COMMANDS
  };

#define PICLANG_SUCCESS 0



#endif //PICLANG_H
