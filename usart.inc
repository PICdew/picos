#define USART_READ_FLAG PIR1,RCIF
#define USART_WRITE_FLAG PIR1,TXIF


;Code for copying to and from the usart_addr location.
;Does not return. Instead it goest to RETURN_ADDR upon completion.
;This is done to be gental to the call stack during an interrupt.
USART_COPY_MAC macro myStatus,usart_addr,PUSH_STACK,POP_STACK,RETURN_ADDR

;Copies the character that is in RCREG to the address stored in usart_addr
;If usart_adder is null, nothing happens.
;Leaves in bank 0
READ_USART movlw 0
	xorwf usart_addr,W
	btfsc STATUS,Z
	goto RETURN_ADDR
	movf FSR,W
	call PUSH_STACK
	banksel usart_addr
	movf usart_addr,W
	movwf FSR
	banksel RCREG
	movf RCREG,W
	movwf INDF
	call POP_STACK
	movwf FSR
	goto RETURN_ADDR
	
;Copies the data in the address stored in usart_addr to TXREG.
;If usart_addr is null, nothing happens.
;Leaves in bank 0
WRITE_USART movlw 0
	xorwf usart_addr,W
	btfsc STATUS,Z
	goto RETURN_ADDR
	movf FSR,W
	call PUSH_STACK
	banksel usart_addr
	movf usart_addr,W
	movwf FSR
	movf INDF,W
	banksel TXREG
	movwf TXREG
	call POP_STACK
	movwf FSR
	goto RETURN_ADDR
	
	endm