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
#define PROCESS_MEMORY_SPACE 7;amount of bytes to be given to all processes
#define MAX_PROCESS_COUNT 5
#define FREE_STORE_LIMIT 0x4C
#define NEXT_POINTER 0
#define PROGRAM_COUNTER 1
#define PROC_EXCHANGE 2
#define PROC_A 3
#define PROC_W 4
#define PROC_STATUS 5
#define PROC_PHYSICAL_BEGINNING 6

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

;Very important memory locations. Memory above 70h is accessible in any bank
	cblock 0x70
	errorByte
	bankSelection
	interruptBankSave
	tmp
	myStatus
	saveW
	programCounter 
	current_process ; memory address of the currently running process's PCB
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
	endc

;Page tabe is in bank 2
      cblock 0xA0
page_table: PAGE_COUNT
page_memory: PAGE_SIZE * PAGE_COUNT
      endc

PAGE_BANK_MAC macro 
PAGE_BANK bsf STATUS,RP1
	bcf STATUS,RP0
	return
	endm

INIT_KERNEL_MAC macro errorByte,PAGE_BANK
	movlw SUCCESS
	movwf errorByte
	clrf wait_queue_head
	clrf wait_queue_tail
	call PAGE_BANK
	movlw low page_table
	movwf FSR
	movlw PAGE_COUNT
	movwf tmp
	movlw 0x0
	movwf INDF
	incf FSR,F
	decfsz tmp,F
	goto $-4
	bcf STATUS,RP0
	bcf STATUS,RP1
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
	
RESUME_PROCESS_MAC macro program_location,accumulator,exchange,programCounter,TERMINATE_PROCESS
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
	movf program_location,W
	movwf FSR
	movlw PROC_STATUS
	addwf FSR,F
	movlw PROC_RUNNING
	movwf INDF
	movf program_location,W
	movwf FSR
	movlw PROC_W
	addwf FSR,F
	movf INDF,W
	return	
	endm

;exchange is used as a temporary location for program locations
;does return.
TERMINATE_PROCESS_MAC macro process_location,exchange,errorByte,ERROR_RETURN
TERMINATE_PROCESS movf process_location,W
	movwf FSR
	movf INDF,W ; process after the one terminated (null if it is the tail)
	movwf exchange
	movf wait_queue_head,W
	movwf FSR
	movf process_location,W
	xorf INDF,W
	btfss STATUS,Z
	goto TERMINATE_PROCESS_HAVE_PRECEEDING_PROCESS
	movf INDF,W
	movwf FSR
	goto $-6
TERMINATE_PROCESS_HAVE_PRECEEDING_PROCESS movf exchange,W ; INDF now points to the process proceeding the terminated process
	movwf INDF
	movf process_location,W
	xorwf wait_queue_tail,W ; if this process is the tail, update tail 
	btfsc STATUS,Z
	goto $+3
	movf INDF,W
	movwf wait_queue_tail
	movlw SUCCESS
	subwf errorByte,W
	btfsc STATUS,Z
	return
	goto ERROR_RETURN
	endm

;Right now, I don't know what to do upon error. So we'll just pretend nothing happend :-(
ERROR_RETURN_MACRO macro errorByte
ERROR_RETURN movlw SUCCESS
	movwf errorByte
	return
	end

;Program binaries start with the amount of memory *pages* they request in free store as the first byte.
;exchange will temporarily hold the memory location where the program is stored (i.e. the actual binary)
START_PROCESS_MAC macro exchange,errorByte,ERROR_RETURN
START_PROCESS movlw PROCESS_MEMORY_SPACE * (MAX_PROCESS_COUNT - 1) ; test to see if there is room at the end of the queue
	subwf wait_queue_tail,W
	btfsc STATUS,C
	goto START_PROCESS_NO_SPACE
	btfsc STATUS,Z
	goto START_PROCESS_NO_SPACE
	movf wait_queue_tail,W
	movwf FSR
	movwf INDF
	movlw PROCESS_MEMORY_SPACE
	addwf INDF,F ; the pointer of the former tail now points to the new process
	movf INDF,W
	movwf wait_tail_queue ; new process is now the tail of the queue
	movwf FSR ; fill new process space with default values
	movlw 0x0
	movwf INDF
	incf FSR,F
	movwf INDF
	movf process_location,W
	movwf FSR
	movlw PROC_STATUS
	addwf FSR,F
	movlw PROC_WAITING
	movwf INDF
	movlw PROC_STATUS
	subwf FSR,F
	movlw PROC_PHYSICAL_BEGINNING
	addwf FSR,F
	movf exchange,W ; starting point of binary code
	movwf INDF	
	call READ_EEPROM ; first byte of binary code is the amount of requested *pages*
	call REQUEST_MEMORY ; new process is in wait_queue_tail
	movlw 0x0
	subwf errorByte ; at least possible to error out because there is no memory available.
	btfsc STATUS,Z
	goto ERROR_RETURN
	return
START_PROCESS_NO_SPACE movlw INSUFFICIENT_PROCESS_QUEUE_MEMORY
        movwf errorByte
	goto ERROR_RETURN
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
	

;process_id equals process_locations.
;will use errorByte as a temporary variable until the end
;where it will be filled with an error code (or success)
TRANSLATE_PAGE_ADDRESS_MAC macro page_addr,process_id,errorByte,temp,PAGE_BANK,ERROR_RETURN
TRANSLATE_PAGE_ADDRESS movlw PAGE_SIZE
		       movwf temp
		       movlw 0x0
		       subwf temp,F
		       btfss STATUS,Z
		       goto $+4
		       movlw INSUFFICIENT_MEMORY
		       movwf errorByte
		       goto ERROR_RETURN
		       movlw 0xf0
		       andwf page_addr,W
		       movwf errorByte
		       swapf errorByte,F
		       call PAGE_BANK
		       movlw low page_table - 1
		       movwf FSR
		       incf FSR,F
		       movlw low page_table + PAGE_COUNT
		       subwf FSR,W
		       btfsc STATUS,Z ; check to see if we've gone page the end of the page table.
		       goto TRANSLATE_PAGE_ADDRESS_ERROR_INVALID_ADDRESS
		       movf process_id,W
		       xorwf INDF,W
		       btfsc STATUS,Z;check to see if the value in the page_table equals process_id
		       decfsz errorByte,F
		       goto $-5
		       movlw 0xf
		       andwf page_addr,W
		       movwf errorByte ; errorByte now contains the index of the memory location within the page
		       movlw low page_table ; FSR now holds (low page_table) plus the page number of the page to which page_addr corresponds
		       subwf FSR,W ; page number that we need to use for page_addr
		       addwf errorByte ; page_number + index
     		       movlw PAGE_SIZE ; we need to multiply page_number + index by the page size. btw PAGE_SIZE is not zero per check at beginning
		       movwf temp
		       movlw 0x1
		       xorwf temp,W
		       btfsc STATUS,Z
		       goto TRANSLATE_PAGE_ADDRESS_FINISH
		       rlf errorByte ; multiply by page size
		       decsz temp,F
		       goto $-2
		       movlw PAGE_SIZE
		       movwf temp
		       btfss temp,0
		       goto TRANSLATE_PAGE_ADDRESS_FINISH
		       movlw low page_table ; in case page size is odd
		       subwf FSR,W
		       addwf errorByte	       
TRANSLATE_PAGE_ADDRESS_FINISH movf errorByte,W
			      movwf temp
			      movlw SUCCESS
			      movwf errorByte
			      movf temp,W
			      return
TRANSLATE_PAGE_ADDRESS_ERROR_INVALID_ADDRESS movlw INVALID_ADDRESS
					     movwf errorByte
					     goto ERROR_RETURN 
			      endm

;returns if there is no process in queue
;if there is a process in the queue, then *goto* RESUME_PROCESS
DISPATCH_NEXT_PROCESS_MAC macro RESUME_PROCESS
DISPATCH_NEXT_PROCESS movlw 0x0
		  subwf wait_queue_head,W
		  btfsc STATUS,Z
		  return
		  goto RESUME_PROCESS
		  endm

		       		       
		       		