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
  char offset;// offset used to find sections of binary
  char bitmap;// flags for the OS, i.e. uses system call arguments?
  char num_pages;
  char pc;// program counter
  char status;// Error status
  char start_address;// First byte of eeprom used for program (after PCB)
  char string_address;// First byte of string data (null terminated)
  char stack[PICLANG_STACK_SIZE];
  char stack_head;
}PCB;
#define PCB_SIZE sizeof(PCB)

extern PCB curr_process;
volatile char PICLANG_quantum;
volatile char PICLANG_system;// CRC hex value for system calls within piclan

extern void PICLANG_init();

/**
 * Loads the n-th program from EEPROM
 * Returns status of load
 */
extern char PICLANG_load(char nth);

/**
 * Saves the program to EEPROM
 * Returns status of program at save.
 */
extern char PICLANG_save(char status);

extern void PICLANG_next();

extern char PICLANG_pop();

enum PICLANG_COMMANDS
  {
    PICLANG_ADD=0, PICLANG_SUB, PICLANG_MULT,PICLANG_PUSHL,PICLANG_PUSH,
    PICLANG_POP, PICLANG_INPUT,PICLANG_PRINT,PICLANG_PRINTL,
    PICLANG_SPRINT,PICLANG_SYSTEM,PICLANG_MORSE,PICLANG_NUM_COMMANDS
  };

enum PICLANG_STATUS{PICLANG_SUCCESS = 0,PICLANG_UNKNOWN_ERROR,PICLANG_NO_SUCH_PROGRAM,
	PICLANG_SUSPENDED,PICLANG_UNKNOWN_COMMAND,PICLANG_PC_OVERFLOW,
	PICLANG_EEPROM_OVERFLOW,PICLANG_STACK_OVERFLOW};



#endif //PICLANG_H
