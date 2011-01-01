;command CRC values
;CRC the char values for each character in the command.
#define EXEC_CRC 42;fill this is later. EXEC runs a process.
#define DEFAULT_SHELL_CHAR 0xfe
#define BACKSPACE_CHAR 0x8
#define CR 0xd
#define LF 0xa


;parses commands
;returns when finished
;Precomputed crc's sound be defined for use in this routine
;If the command has arguments, they are pushed into the stack, from left to right
SHELL_MAC macro lcd_pointer,lcd_buffer,crc,usart_addr,current_process,PARSE_ARGS,CRC_W,PUSH_STACK,CLEAR_LCD_BUFFER,INPUT_METHOD,EXEC,WRITE_BUFFER
SHELL movlw SHELL_ID
	banksel lcd_owner
	xorwf lcd_owner,W ; Is the shell aready running?
	btfsc STATUS,Z
	goto SHELL_UPDATE_CHECK
	call CLEAR_LCD_BUFFER ; INIT SHELL
	banksel lcd_owner
	movlw SHELL_ID
	movwf lcd_owner
	movlw low lcd_buffer
	movwf FSR
	movlw '?'
	movwf INDF
	incf FSR,F
	movlw DEFAULT_SHELL_CHAR
	movwf INDF
	movf FSR,W
	banksel lcd_pointer
	movwf lcd_pointer
	banksel usart_addr
	movwf usart_addr
	bcf STATUS,RP0
	bcf STATUS,RP1
	goto SHELL_UPDATE_DISPLAY
SHELL_UPDATE_CHECK nop
	banksel lcd_pointer; if so, should it update itself?
	movf lcd_pointer,W
	movwf FSR
	movlw DEFAULT_SHELL_CHAR ; default character for shell. Did it change?
	xorwf INDF,W
	btfsc STATUS,Z
	goto END_OF_SHELL
	movlw BACKSPACE_CHAR ; is the character a backspace, if so remove character
	xorwf INDF,W
	btfsc STATUS,Z
	goto SHELL_BACKSPACE
	movlw CR ; Let's not retain CR. Windows :-/
	xorwf INDF,W
	btfsc STATUS,Z
	goto SHELL_NO_CR
	movlw LF
	xorwf INDF,W
	btfsc STATUS,Z
	goto DO_STUFF_IN_SHELL
SHELL_ADVANCE_UPDATE incf FSR,F ; Advance lcd pointer
	movlw DEFAULT_SHELL_CHAR
	movwf INDF
	movf FSR,W
	movwf lcd_pointer
	banksel usart_addr
	movwf usart_addr
	bcf STATUS,RP0
	bcf STATUS,RP1
SHELL_UPDATE_DISPLAY movlw SHELL_ID
	call PUSH_STACK
	call WRITE_BUFFER
END_OF_SHELL return
SHELL_BACKSPACE movlw DEFAULT_SHELL_CHAR
	movwf INDF
	decf FSR,F
	movwf INDF
	movf FSR,W
	banksel usart_addr
	movwf usart_addr
	banksel lcd_pointer
	movwf lcd_pointer
	goto SHELL_UPDATE_DISPLAY
DO_STUFF_IN_SHELL movlw '!'
	movwf INDF
	goto SHELL_ADVANCE_UPDATE	
SHELL_NO_CR movlw DEFAULT_SHELL_CHAR
	movwf INDF
	goto SHELL_UPDATE_DISPLAY

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