#ifndef PICLANG_H
#define PICLANG_H 1

#define EOP 0xff // end of program

#ifndef DEFAULT_PICLANG_QUANTUM
#define DEFAULT_PICLANG_QUANTUM 25
#endif

#ifndef PICLANG_STACK_SIZE
#define PICLANG_STACK_SIZE 0x10
#endif

enum PICLANG_STATUS{PICLANG_SUCCESS = 0,PICLANG_UNKNOWN_ERROR,PICLANG_NO_SUCH_PROGRAM,PICLANG_SUSPENDED,PICLANG_UNKNOWN_COMMAND,PICLANG_PC_OVERFLOW,PICLANG_EEPROM_OVERFLOW,PICLANG_STACK_OVERFLOW};

typedef struct{
  char size;// size of program
  char pc;// program counter
  char status;// Error status
  char start_address;// First bit off eeprom used for program (after PCB)
  char stack[PICLANG_STACK_SIZE];
  char stack_head;
}PCB;
#define PCB_SIZE 5 + PICLANG_STACK_SIZE

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
extern char PICLANG_save();

extern void PICLANG_next();

enum PICLANG_COMMANDS
  {
    PICLANG_ADD=0, PICLANG_SUB, PICLANG_MULT,PICLANG_PUSHL,PICLANG_PUSH,
    PICLANG_POP, PICLANG_INPUT,PICLANG_PRINT,PICLANG_PRINTL,
    PICLANG_SYSTEM,PICLANG_NUM_COMMANDS
  };

#endif //PICLANG_H
