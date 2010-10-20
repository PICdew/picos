;picos kernel
;author: David Coss
;Date: 10/2/10
;License: GNU Public License version 3.0 (see http://www.gnu.org)
;
;Kernel will:
;
; 0.1.0 - Created process queue system. Processes will have their own portion of memory.
;		process will run for a certain amount of time and then save their state
;		and allow another process to run, given the illusion of multitasking.
;		Process rotation occurs on interrupt. Processes also can allocate
;		memory in a "free store". This feature needs to be added to piclang.
;		Also, a garbage collector will be come necessary.
;
; 0.0.0 - Reserve memory starting at 70h (see below for length of reserved memory).
;	- Create a stack that will end at 6fh to be used to store program registers
;		if a process is suspended. Size is determined by free space, which is equal
;		to 0x6f - CALL_STACK_LIMIT.

#define KERNEL_MAJOR_VERSION 0x0
#define KERNEL_MINOR_VERSION 0x1
#define KERNEL_REVISION 0x0

;Process defines
#define PROCESS_MEMORY_SPACE 8;amount of bytes to be given to all processes
#define MAX_PROCESS_COUNT 5
#define FREE_STORE_LIMIT 0x4C
#define NEXT_POINTER 0
#define PROGRAM_COUNTER 1
#define PROC_EXCHANGE 2
#define PROC_A 3
#define PROC_W 4
#define MEM_START 5
#define MEM_LIMIT 6
#define PROC_STATUS 7

;Process states
#define PROC_RUNNING 0
#define PROC_WAITING 1
#define PROC_ENDED   2

;Error valules written to errorByte
#define SUCCESS 0
#define NO_PROCESSES_TO_RUN 1;can be used to say that the call stack is empty

;Very important memory locations. Memory above 70h is accessible in any bank
	cblock 0x70
	errorByte
	bankSelection
	interruptBankSave
	tmp
	myStatus
	saveW
	programCounter
	current_process
	pclTemp
	accumulator
	exchange
	eepromSize
	instruction
	stackPtr
	endc

;Process space
	cblock 0x20
wait_queue_head
wait_queue_tail
process_space: PROCESS_MEMORY_SPACE * MAX_PROCESS_COUNT
free_store: FREE_STORE_LIMIT - PROCESS_MEMORY_SPACE * MAX_PROCESS_COUNT - 2;give remaining space to free store
	endc

INIT_KERNEL_MAC macro errorByte
	clrf errorByte
	clrf wait_queue_head
	clrf wait_queue_tail
	endm

SUSPEND_PROCESS_MAC macro process_location,accumulator,exchange,programCounter,errorByte,TERMINATE_PROCESS
SUSPEND_PROCESS movwf saveW;save the process's W-reg
	movf process_location,W
	movwf FSR
	movlw PROC_W
	addwf FSR,F
	movf saveW,W
	movwf INDF
	movlw SUCCESS ;check to see if the process has errored out
	xorwf errorByte,W
	btfsc STATUS,Z
	goto TERMINATE_PROCESS
	movf process_location,W ;no errors; save state
	movwf FSR
	incf FSR,F
	movf programCounter,W
	movwf INDF
	incf FSR,F
	movf exchange,W
	movwf INDF
	incf FSR,F
	movf accumulator,W
	movwf INDF
	movf process_location,W
	movwf FSR
	movlw FROC_STATUS
	addwf FSR,F
	movlw PROC_WAITING
	movwf INDF
	movf wait_queue_tail,W
	movwf FSR
	movf program_location,F
	movwf INDF
	movwf wait_queue_tail
	return
	endm
	
RESUME_PROCESS_MAC macro accumulator,exchange,programCounter,TERMINATE_PROCESS
RESUME_PROCESS movf wait_queue_head,W
	movwf program_location
	movwf FSR
	movf INDF,W
	movwf wait_queue_head
	movf program_location,W
	movwf FSR
	incf FSR,F
	movf INDF,W
	movwf programCounter
	incf FSR,F
	movf INDF,W
	movwf exchange
	incf FSR,F
	movf INDF,W
	movwf accumulator
	movf programLocation,W
	movwf FSR
	movlw PROC_STATUS
	addwf FSR,F
	movlw PROC_RUNNING
	movwf INDF
	movf programLocation,W
	movwf FSR
	movlw PROC_W
	addwf FSR,F
	movf INDF,W
	return	
	endm


START_INTERRUPT macro saveW,interruptBankSave,bankSelect,SAVE_BANK,SUSPEND_PROCESS
	movwf saveW;save w value
	movf bankSelect,W
	movwf interruptBankSave
	call SAVE_BANK
	call SUSPEND_PROCESS
	endm
	
FINISH_INTERRUPT macro saveW,interruptBankSave,bankSelection,RESET_BANK,RESTART_PROCESS
	movf saveW,W
	call RESET_BANK
	movf interruptBankSave,W
	movwf bankSelection
	bcf INTCON,2
	call RESTART_PROCESS	
	endm

MAIN_PROCESS_MAC macro CREATE_DISPLAY,DISPLAY_ME,controlPort,PROGRAM_MODE,MAIN_LOOP
MAIN_PROCESS call CREATE_DISPLAY
	movlw 0xff
	movwf counter
	call DISPLAY_ME
	decfsz counter,F
	goto $-2
	clrwdt;WDT checks for run away code. If I get this far, I'm not running away
PROGRAM_MAIN_FORK	btfsc controlPort,PROGRAM_MODE_BIT
	goto PROGRAM_MODE
	goto MAIN_LOOP ; repeat....
	endm
	
BANK_MASK_MAC macro bankSelection
SAVE_BANK clrf bankSelection
	btfsc STATUS,RP0
	bsf bankSelection,RP0
	btfsc STATUS,RP1
	bcf bankSelection,RP1
	return
	
RESET_BANK movf bankSelection,W
	iorwf STATUS,F
	return
	endm
	
	
