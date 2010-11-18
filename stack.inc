;picos stack code
;Author: David Coss
;Date: 10/1/10
;License: GPL v3.0 (http://www.gnu.org)
#define STACK_HEAD_ADDR low stackHead

INIT_STACK_MAC macro headaddr,stackPtr 
	movlw low headaddr;initialize stack
	movwf stackPtr
	endm

;pops the stack onto W-reg
POP_STACK_MAC macro stackTemp,stackTemp2,stackPtr
POP_STACK movf FSR,W ;save the FSR value before popping
	movwf stackTemp
	movf stackPtr,W
	movwf FSR
	movf INDF,W ; get value at top of stack
	movwf stackTemp2 ; temporarily store the popped value
	movf stackTemp,W ; restore previous FSR
	movwf FSR
	movf stackTemp2,W ; popped value is now in w-reg
	decf stackPtr,F
	return
	endm

;Returns with the value at the top of the stack in w-reg.
;Unlike pop, peek leaves the data in the stack.
PEEK_STACK_MAC macro stackTemp,stackTemp2,stackPtr
PEEK_STACK call POP_STACK
	incf stackPtr,F
	return
	endm


	;
;push W-reg onto the stack
PUSH_STACK_MAC macro stackTemp,stackTemp2,stackPtr 
PUSH_STACK movwf stackTemp 
	movf FSR,W
	movwf stackTemp2
	incf stackPtr,F
	movf stackPtr,W
	movwf FSR
	movf stackTemp,W
	movwf INDF
	movf stackTemp2,W
	movwf FSR
	movf stackTemp,W
	return
	endm
	
