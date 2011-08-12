#define ARG_SIZE 17

enum {ARG_SHELL,ARG_PICLANG};
char ARG_next;// index into ARG_buffer 
char ARG_end;
char ARG_source;// source of arguments, i.e. shell or piclang

#define ARG_EOA() (ARG_next >= ARG_SIZE)

/**
 * Clears argument buffer
 */
void ARG_clear();

/**
 * Returns the value of the next argument, where the argument is a 
 * decimal non-negative integer.
 *
 * If the next argument is not a non-negative integer, -1 is returned
 * and error_code is set.
 */
signed char ARG_getd();

void ARG_putch(char ch);
char ARG_getch();
const char* ARG_gets();
