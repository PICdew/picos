#define USART_READ_FLAG PIR1,RCIF
#define USART_WRITE_FLAG PIR1,TXIF

#define SSP_STAT_FLAG SSPSTAT, BF

INIT_USART macro usart_addr
	banksel usart_addr
	clrf usart_addr
	bcf STATUS,RP1
	bsf STATUS,RP0
	movlw	b'1010000'		;set tris bits for TX and RX
	iorwf	TRISB,F
	movlw	.5	;set baud rate
	movwf	SPBRG
	movlw	0x24		;enable transmission and high baud rate
	movwf	TXSTA
	Bank0			;select bank0
	movlw	0x90		;enable serial port and reception
	movwf	RCSTA

	movlw	0xc0		;enable global and peripheral ints
	movwf	INTCON
	Bank1			;select bank1
	movlw	0x30		;enable TX and RX interrupts
	movwf	PIE1
	endm

;Code for copying to and from the usart_addr location.
;Does not return. Instead it goest to RETURN_ADDR upon completion.
;This is done to be gental to the call stack during an interrupt.
USART_COPY_MAC macro myStatus,usart_addr,errorByte,PUSH_STACK,POP_STACK,RETURN_ADDR

;Copies the character that is in RCREG to the address stored in usart_addr
;If usart_adder is null, nothing happens.
;Leaves in bank 0
READ_USART movlw 0
	xorwf usart_addr,W
	btfsc STATUS,Z
	goto RETURN_ADDR
	btfsc	RCSTA,OERR	;check overrun error flag
	goto	ERROR_OVRRUN		
	btfsc	RCSTA,FERR	;test framing error flag
	goto	ERROR_FRAME	
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
ERROR_OVRRUN bcf RCSTA,CREN
	bsf RCSTA,CREN
	movlw USART_ERROR
	movwf errorByte
	goto RETURN_ADDR
ERROR_FRAME	movf RCREG,W
	movlw USART_ERROR
	movwf errorByte
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
	
;Copies the SSPBUF to the desired memory location and then copies the next
;transmit location to SSPBUF, in effect syncronizing the read/write process
;between user mode processes and the SSP process.
;
;Does not return. Instead goes to RETURN_ADDR after completion.
;Returns in bank 0
SYNC_SSP_MAC macro rx_addr,tx_addr,PUSH_STACK,POP_STACK,RETURN_ADDR
SYNC_SSP movf FSR,W
	call PUSH_STACK
	banksel rx_addr
	movlw 0
	xorwf rx_addr,W
	btfsc STATUS,Z
	goto SYNC_TRANSMIT_SSP
	movwf FSR
	banksel SSPBUF
	movf SSPBUF,W
	movwf INDF
SYNC_TRANSMIT_SSP movlw 0
	banksel tx_addr
	xorwf tx_addr,W
	btfsc STATUS,Z
	goto END_SYNC_SSP
	movwf FSR
	banksel SSPBUF
	movf INDF,W
	movwf SSPBUF
	bcf myStatus,SSP_WRITE_BUSY_FLAG
END_SYNC_SSP goto RETURN_ADDR
	endm