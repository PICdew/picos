;picos kernel 0.0.0
;author: David Coss
;Date: 10/2/10
;License: GNU Public License version 3.0 (see http://www.gnu.org)
;
#define KERNEL_MAJOR_VERSION 0
#define KERNEL_MINOR_VERSION 0
#define KERNEL_REVISION 0

#define CALL_STACK_LIMIT low endOfMemory
END_OF_PHYSICAL_MEMORY equ 6fh

;Error valules written to errorByte
#define CALL_STACK_OVERFLOW 1
#define CALL_STACK_UNDERFLOW 2


errorByte equ END_OF_PHYSICAL_MEMORY
callStackPtr equ errorByte - 1

INIT_KERNEL_MAC errorByte
	clrf errorByte
	endm

;call stack Routines to be used by pic os
;thePtr contains the current memory 
;location of the top of the stack.
;the value of thePtr increases when
;the stack is popped and decreases
;upon push.
;Return locations should be stored
;as two bytes in the stack, with
;the upper byte at the top, because
;pics will have address greater than
;255
INIT_CALL_STACK_MAC macro thePtr,MAIN_LOOP
	movlw low MAIN_LOOP
	call PUSH_CALL_STACK
	movlw high MAIN_LOOP
	call PUSH_CALL_STACK
	endm
	
POP_CALL_STACK_MAC macro thePtr,ERROR_RETURN
POP_CALL_STACK movlw (low END_OF_PHYSICAL_MEMORY) + 1
	subwf thePtr,W
	btfss STATUS,Z
	goto $+3
	movlw CALL_STACK_OVERFLOW
	goto ERROR_RETURN
	movf thePtr,W
	movwf FSR
	movf INDF,W
	incf thePtr,F
	endm
	
;uses stack to temporarily store the value to
;be pushed.
PUSH_CALL_STACK_MAC macro thePtr,PUSH_STACK,POP_STACK,ERROR_RETURN,endOfMemory
PUSH_CALL_STACK call PUSH_STACK
	incf thePtr,F
	movlw low endOfMemory
	subf thePtr,W
	btfss STATUS,Z
	goto $+3
	movlw CALL_STACK_OVERFLOW
	goto ERROR_RETURN
	movf thePtr,W
	movwf FSR
	call POP_STACK
	movwf INDF
	return
	endm
	
;Reloads programCounter, exchange, accumulator and W from the stack (which are in 
;that order from top to bottom). Then the routine goto's  RUN_PROGRAM
RESTART_PROCESS_MAC macro POP_CALL_STACK,accumulator,exchange,programCounter,RUN_PROGRAM
RESTART_PROCESS call POP_CALL_STACK
	movwf programCounter
	call POP_CALL_STACK
	movwf exchange
	call POP_CALL_STACK
	movwf accumulator
	call POP_CALL_STACK
	goto RUN_PROGRAM
	endm
	
;Saves the program state.
SUSPEND_PROCESS_MAC macro PUSH_CALL_STACK,accumulator,exchange,programCounter
SUSPEND_PROCESS	call PUSH_STACK
	movf accumulator,W
	call PUSH_STACK
	movf exchange,W
	call PUSH_STACK
	movf programCounter,W
	call PUSH_STACK
	return
	endm


START_INTERRUPT macro saveW,SUSPEND_PROCESS
	movwf saveW;save w value
	call SUSPEND_PROCESS
	endm
	
FINISH_INTERRUPT macro saveW
	movf saveW,W
	bcf INTCON,2
	call RESUME_PROCESS
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
	
	
	
