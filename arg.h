#define ARG_SIZE 17


char ARG_buffer[ARG_SIZE];
char ARG_next;// index into ARG_buffer 
#define ARG_EOA() (ARG_next >= ARG_SIZE)

/**
 * Clears argument buffer
 */
void ARG_clear();

char ARG_next_int();

void ARG_putch(char ch);
char ARG_getch();
