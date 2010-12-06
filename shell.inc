;command CRC values
;CRC the char values for each character in the command.
#define EXEC_CRC 42;fill this is later. EXEC runs a process.

;parses commands
;returns when finished
;Precomputed crc's sound be defined for use in this routine
;If the command has arguments, they are pushed into the stack, from left to right
SHELL_MAC macro lcd_pointer,lcd_buffer,crc,usart_addr,PARSE_ARGS,CRC_W,PUSH_STACK,CLEAR_LCD_BUFFER,INPUT_METHOD,EXEC,WRITE_BUFFER
SHELL call CLEAR_LCD_BUFFER
	movf lcd_pointer,W
	btfsc STATUS,Z
	movlw low lcd_buffer
	movwf FSR
	movlw '?'
	movwf INDF
	incf FSR,W
	movwf lcd_pointer
	call WRITE_BUFFER
SHELL_LOOP movf lcd_pointer,W
	movwf FSR
	movlw ' '
	xorwf INDF,W
	btfss STATUS,Z ; if the buffer value differs from space, it has changed; thus, update the display.
	goto SHELL_DISPLAY_UPDATE
	banksel usart_addr
	movf FSR,W
	movwf usart_addr
	banksel lcd_pointer
	movlw 0x0a;LF
	xorwf INDF,W ; need to add LF to MORSE_TABLE!!! Possibly RTN without dit or dat???
	btfsc STATUS,Z
	goto READ_COMMAND
	call INPUT_METHOD
	goto SHELL_LOOP
SHELL_DISPLAY_UPDATE call WRITE_BUFFER
	incf lcd_pointer,F
	goto SHELL_LOOP
	

READ_COMMAND movlw low lcd_buffer ; plus one for the ?character
	addlw 1
	movwf FSR
	banksel crc
	clrf crc
READ_COMMAND_LOOP movf INDF,F ;double check this can set Z flag
	btfsc STATUS,Z
	goto HAVE_COMMAND
	movlw ' '
	xorwf INDF,W
	btfsc STATUS,Z
	goto PARSE_ARGS
	movf INDF,W
	call CRC_W
	incf FSR,F
	goto READ_COMMAND_LOOP
PARSE_ARGS incf FSR,F
	movf INDF,F 
	btfsc STATUS,Z
	goto HAVE_COMMAND
	movlw ' '
	xorwf INDF,W
	goto PARSE_ARGS
	movf INDF,W
	call PUSH_STACK
	goto PARSE_ARGS
HAVE_COMMAND movlw EXEC_CRC
	xorwf crc,W
	btfsc STATUS,Z
	goto EXEC
	return
	endm