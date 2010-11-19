;Process Control Block (PCB) defines
#define PROCESS_MEMORY_SPACE 7;amount of bytes to be given to all processes for their PCB
#define MAX_PROCESS_COUNT 5
#define FREE_STORE_LIMIT 0x4C
#define PROGRAM_BINARY_OFFSET 2;position of the first byte of program op codes, i.e. first actual line of the program and not header. this is zero-indexed.
#define NEXT_POINTER 0
#define PROGRAM_COUNTER 1
#define PROC_EXCHANGE 2
#define PROC_A 3
#define PROC_W 4
#define PROC_STATUS 5
#define PROC_PHYSICAL_BEGINNING 6 ; location of the first byte of the binary in secondary storage.

;Process states
#define PROC_RUNNING 0
#define PROC_WAITING 1
#define PROC_ENDED   2

;Page Defines
#define PAGE_SIZE 5
#define PAGE_COUNT 5

;Error valules written to errorByte
#define SUCCESS 0
#define NO_PROCESSES_TO_RUN 1;can be used to say that the call stack is empty
#define INSUFFICIENT_PROCESS_QUEUE_MEMORY 2 ; when a process cannot be put in the queue. It's full.
#define INSUFFICIENT_MEMORY 3 ; no more memory in free store
#define INVALID_ADDRESS 4 ; page address is not a valid location
#define NO_SUCH_COMMAND 5
#define BEYOND_PROGRAM_MEMORY 6
#define USART_ERROR 7
#define LCD_REQUEST_DENIED 8
#define INTEGER_OVERFLOW 9

;myStatus defines
#define ALARM_FLAG 0
#define UPDATED_TIME 1
#define USART_BUSY_FLAG 2
#define SSP_READ_BUSY_FLAG 3
#define SSP_WRITE_BUSY_FLAG 4

;Display status bits
#define FLUSH_BUFFER 0
