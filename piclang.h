#ifndef PICLANG_H
#define PICLANG_H 1

#define EOP 0xff // end of program

#ifndef DEFAULT_PICLANG_QUANTUM
#define DEFAULT_PICLANG_QUANTUM 25
#endif

enum PICLANG_STATUS{SUCCESS = 0,UNKNOWN_ERROR,NO_SUCH_PROGRAM,SUSPENDED,UNKNOWN_COMMAND,PC_OVERFLOW,EEPROM_OVERFLOW};

typedef struct{
  char size;// size of program
  char pc;// program counter
  char A;// Accumulator
  char W;// w-reg
  char status;// Error status
  char start_address;// First bit off eeprom used for program (after PCB)
}PCB;

extern PCB curr_process;
extern char PICLANG_quantum;


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

enum PICLANG_COMMANDS{PICLANG_ADDA, PICLANG_SUBA, PICLANG_MULTA};

#endif //PICLANG_H
