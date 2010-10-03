;picos stack code
;Author: David Coss
;Date: 10/1/10
;License: GPL v3.0 (http://www.gnu.org)
INIT_STACK_MAC macro headaddr,stackPtr 
	movlw low headaddr;initialize stack
	movwf stackPtr
	endm

;pops the stack onto W-reg
POP_STACK_MAC macro stackPtr
POP_STACK movf stackPtr,W
	movwf FSR
	decf stackPtr,F
	movf INDF,W
	return
	endm
	;
;push W-reg onto the stack
PUSH_STACK_MAC macro stackTemp,stackPtr 
PUSH_STACK movwf stackTemp 
	incf stackPtr,F
	movf stackPtr,W
	movwf FSR
	movf stackTemp,W
	movwf INDF
	return
	endm
	
