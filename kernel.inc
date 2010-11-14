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
#define KERNEL_MINOR_VERSION 0x2
#define KERNEL_REVISION 0x1

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

;myStatus defines
#define ALARM_FLAG 0
#define UPDATED_TIME 1
#define USART_BUSY_FLAG 2
#define SSP_READ_BUSY_FLAG 3
#define SSP_WRITE_BUSY_FLAG 4

;Display status bits
#define FLUSH_BUFFER 0

;Very important memory locations. Memory above 70h is accessible in any bank
	cblock 0x70
	errorByte
	bankSelection
	interruptBankSave
	temp
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
	stackTemp
	stackTemp2
	endc

;Process space
	cblock 0x20
wait_queue_head
wait_queue_tail
pcb_block: PROCESS_MEMORY_SPACE * MAX_PROCESS_COUNT
	endc

;Page tabe is in bank 1
      cblock 0xA0
page_table: PAGE_COUNT
page_memory: PAGE_SIZE * PAGE_COUNT
free_page_count 
      endc

PAGE_BANK_MAC macro 
PAGE_BANK bsf STATUS,RP1
	bcf STATUS,RP0
	return
	endm

INIT_KERNEL_MAC macro errorByte,free_page_count,current_process,PAGE_BANK
	movlw SUCCESS
	movwf errorByte
	clrf wait_queue_head
	clrf wait_queue_tail
	call PAGE_BANK
	movlw low page_table
	movwf FSR
	movlw PAGE_COUNT
	movwf temp
	clrf INDF
	incf FSR,F
	decfsz temp,F
	goto $-3
	movlw free_page_count
	movwf FSR
	movlw PAGE_COUNT
	movwf INDF
	banksel lcd_owner
	clrf lcd_owner
	bcf STATUS,RP0
	bcf STATUS,RP1
	clrf current_process
	endm

SUSPEND_PROCESS_MAC macro process_location,accumulator,exchange,programCounter,errorByte,TERMINATE_PROCESS
SUSPEND_PROCESS movwf saveW;save the process's W-reg
	movf process_location,W
	xorlw 0
	btfsc STATUS,Z
	return
	movwf FSR
	movlw PROC_W
	addwf FSR,F
	movf saveW,W
	movwf INDF
	movlw SUCCESS ;check to see if the process has errored out
	banksel errorByte
	xorwf errorByte,W
	btfsc STATUS,Z
	goto TERMINATE_PROCESS
	nop
	movf process_location,W ;no errors; save state
	movwf FSR
	incf FSR,F
	banksel programCounter
	movf programCounter,W
	movwf INDF
	incf FSR,F
	bcf STATUS,RP0
	movf exchange,W
	bsf STATUS,RP0
	movwf INDF
	incf FSR,F
	bcf STATUS,RP0
	movf accumulator,W
	bsf STATUS,RP0
	movwf INDF
	bcf STATUS,RP0
	movf process_location,W
	movwf FSR
	bsf STATUS,RP0
	movlw PROC_STATUS
	addwf FSR,F
	movlw PROC_WAITING
	movwf INDF
	bcf STATUS,RP0
	movf wait_queue_tail,W
	movwf FSR
	movf process_location,F
	movwf INDF
	movwf wait_queue_tail
	clrf process_location
	bcf STATUS,RP0
	bcf STATUS,RP1
	return
	endm
	
RESUME_PROCESS_MAC macro quantum,program_location,accumulator,exchange,programCounter,TERMINATE_PROCESS
RESUME_PROCESS movlw DEFAULT_QUANTUM
	movwf quantum
	movf wait_queue_head,W
	movwf program_location
	movwf FSR
	movf INDF,W
	banksel wait_queue_head
	movwf wait_queue_head
	movlw 0
	xorwf wait_queue_head,W
	btfsc STATUS,Z
	movwf wait_queue_tail
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
	bcf STATUS,RP0
	bcf STATUS,RP1
	return	
	endm

;Terminate needs to free paged memory. When a program is RESUMED
;it leaves the wait queue. So TERMINATE_PROCESS does not have to
;do anything to the queue. The schedule will start the next process (if any).
;
;It is terminate process's job to release resources, such as the
;Display.
TERMINATE_PROCESS_MAC macro process_location,errorByte,ERROR_RETURN,FREE_PAGES
TERMINATE_PROCESS movf process_location,W
	call FREE_PAGES
	movf process_location,W
	banksel lcd_owner
	xorwf lcd_owner,W
	btfsc STATUS,Z
	clrf lcd_owner
	banksel process_location
	movlw SUCCESS ; See if the program errored. If so goto error return.
	xorwf errorByte,W
	btfsc STATUS,Z
	return
	goto ERROR_RETURN
	endm

;Right now, I don't know what to do upon error. So we'll just pretend nothing happend :-(
ERROR_RETURN_MAC macro errorByte,DISPLAY_ERROR_STATUS
ERROR_RETURN call DISPLAY_ERROR_STATUS
	movlw SUCCESS
	movwf errorByte
	return
	endm

;Program binaries start with the amount of memory *pages* they request in free store as the first byte.
;exchange will temporarily hold the memory location where the program is stored (i.e. the actual binary)
START_PROCESS_MAC macro errorByte,wait_queue_head,wait_queue_tail,PUSH_STACK,POP_STACK,REQUEST_MEMORY,ERROR_RETURN
START_PROCESS bcf STATUS,RP0
	bcf STATUS,RP1
	movlw 0
	xorwf wait_queue_head,W
	btfsc STATUS,Z
	goto PUSH_FIRST_PCB
	movlw (low pcb_block) + PROCESS_MEMORY_SPACE * (MAX_PROCESS_COUNT - 1) ; test to see if there is room at the end of the queue
	subwf wait_queue_tail,W
	btfsc STATUS,C
	goto START_PROCESS_NO_SPACE
	btfsc STATUS,Z
	goto START_PROCESS_NO_SPACE
ADJUST_QUEUE_HEAD movf wait_queue_tail,W
	movwf FSR
	addlw PROCESS_MEMORY_SPACE
	movwf INDF
	movwf wait_queue_tail
INIT_PCB_VALUES movf wait_queue_tail,W ; new process is now the tail of the queue
	movwf FSR ; fill new process space with default values
	movlw 0x0
	movwf INDF
	incf FSR,F
	movwf INDF
	decf FSR,F
	movlw PROC_STATUS
	addwf FSR,F
	movlw PROC_WAITING
	movwf INDF
	movlw PROC_STATUS
	subwf FSR,F
	movlw PROC_PHYSICAL_BEGINNING
	addwf FSR,F
	call POP_STACK ; starting point of binary code
	bsf STATUS,RP0
	movwf INDF
	bcf STATUS,RP0
	movf wait_queue_tail,W
	call PUSH_STACK ; must be on stack for REQUEST_MEMORY
	addlw PROC_PHYSICAL_BEGINNING
	movwf FSR
	movf INDF,W
	call PUSH_STACK
	call READ_EEPROM ; first byte of binary code is the amount of requested *pages*
	call POP_STACK
	call REQUEST_MEMORY ; new process is in wait_queue_tail
	movlw SUCCESS
	xorwf errorByte ; at least possible to error out because there is no memory available.
	btfss STATUS,Z
	goto ERROR_RETURN
	return
START_PROCESS_NO_SPACE movlw INSUFFICIENT_PROCESS_QUEUE_MEMORY
    movwf errorByte
	goto ERROR_RETURN
PUSH_FIRST_PCB movlw low pcb_block
	movwf wait_queue_head
	movwf wait_queue_tail
	movlw PROCESS_MEMORY_SPACE
	goto INIT_PCB_VALUES
	endm	
	

START_INTERRUPT macro saveW,interruptBankSave,bankSelect,SAVE_BANK
	movwf saveW;save w value
	movf bankSelect,W
	movwf interruptBankSave
	call SAVE_BANK
	endm
	
FINISH_INTERRUPT macro saveW,interruptBankSave,bankSelection,RESET_BANK
	call RESET_BANK
	movf interruptBankSave,W
	movwf bankSelection
	movf saveW,W
	bcf INTCON,GIE
	endm

MAIN_PROCESS_MAC macro CREATE_DISPLAY,DISPLAY_ME,controlPort,PROGRAM_MODE,MAIN_LOOP
MAIN_PROCESS call CREATE_DISPLAY
	call DISPLAY_ME
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
	bsf bankSelection,RP1
	return
	
RESET_BANK bcf STATUS,RP0
	bcf STATUS,RP1
	btfsc bankSelection,RP0
	bsf STATUS,RP0
	btfsc bankSelection,RP1
	bsf STATUS,RP1
	return
	endm

;Checks to see if there are W empty pages in memory.
;If there are, the process_id (which should be in the stack when 
; this routine is called) is placed in page_table
; in the first W entries where the value is zero.
;If there are not W values empty,
REQUEST_MEMORY_MAC macro errorByte,free_page_count,temp,POP_STACK,PUSH_STACK
REQUEST_MEMORY xorlw 0 ; if we request zero memory, leave
	btfsc STATUS,Z
	return
	call PUSH_STACK
	banksel free_page_count
	subwf free_page_count,W
	bcf STATUS,RP0
	bcf STATUS,RP1
	btfsc STATUS,Z
	goto REQUEST_MEMORY_INS_MEM
	btfss STATUS,C
	goto REQUEST_MEMORY_INS_MEM
	call POP_STACK
	movwf temp
	movlw low page_table
	movwf FSR
CLAIM_MEMORY_LOOP movlw 0
	xorwf INDF,W
	btfsc STATUS,Z
	goto CLAIM_MEMORY
	incf FSR,F
CLAIM_MEMORY_LOOP_NEXT decfsz temp,F
	goto CLAIM_MEMORY_LOOP
	goto END_OF_REQUEST_MEMORY
CLAIM_MEMORY call POP_STACK
	movwf INDF
	call PUSH_STACK
	incf FSR,F
	bsf STATUS,RP0
	decf free_page_count,F
	bcf STATUS,RP0
	goto CLAIM_MEMORY_LOOP_NEXT
REQUEST_MEMORY_INS_MEM movlw INSUFFICIENT_MEMORY
	movwf errorByte
END_OF_REQUEST_MEMORY 	call POP_STACK
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
			   bsf STATUS,RP0
		       andwf page_addr,W
			   bcf STATUS,RP0
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

;Moves through the processes and removes pages whos owners
;equal the value initially in W
FREE_PAGES_MAC macro page_table_addr,free_page_count,count,temp
FREE_PAGES movwf temp
	movlw low page_table_addr
	movwf FSR
	movlw PAGE_COUNT
	movwf count
FREE_PAGES_LOOP movf temp,W ; move through page table. see if the owner is the value passed to W-reg
	xorwf INDF,W
	btfss STATUS,Z
	goto $+6
	clrf INDF
	bsf STATUS,RP0
	incf free_page_count,F
	bcf STATUS,RP0
	incf FSR,F
	decfsz count,F
	goto FREE_PAGES_LOOP
	return
    endm

;returns if there is no process in queue
;if there is a process in the queue, then *goto* RESUME_PROCESS
DISPATCH_NEXT_PROCESS_MAC macro wait_queue_head,current_process,RESUME_PROCESS
DISPATCH_NEXT_PROCESS movlw 0x0
	xorwf wait_queue_head,W
	btfss STATUS,Z
	goto RESUME_PROCESS
	movlw NO_PROCESSES_TO_RUN
	movwf errorByte
	clrf current_process
	return
	endm
	
		       		       
		       		