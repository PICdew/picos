
;command CRC values
;CRC the char values for each character in the command.
#define EXEC_CRC 0x3b;fill this is later. EXEC runs a process.
#define TIME_CRC 0x66
#define SET_TIME_CRC 0xea ; set time command

#define DEFAULT_SHELL_CHAR 0xfe
#define BACKSPACE_CHAR 0x7f
#define BACKSPACE_CHAR_KEYBOARD 8
#define CR 0xd
#define LF 0xa


;parses commands
;returns when finished
;Precomputed crc's sound be defined for use in this routine
;If the command has arguments, they are pushed into the stack, from left to right
SHELL_MAC macro lcd_pointer,lcd_buffer,crc,usart_addr,current_process,PARSE_ARGS,CALCULATE_CRC,PUSH_STACK,CLEAR_LCD_BUFFER,INPUT_METHOD,EXEC,WRITE_BUFFER,MULT_BYTE_10
SHELL_INIT call CLEAR_LCD_BUFFER ; INIT SHELL
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
	goto SHELL_UPDATE_DISPLAY; will return from there

SHELL movlw SHELL_ID
	banksel lcd_owner
	xorwf lcd_owner,W ; Is the shell aready running?
	btfsc STATUS,Z
	goto SHELL_UPDATE_CHECK
	movf lcd_owner,F ; If no one owns the LCD, let the shell take over.
	btfsc STATUS,Z
	goto SHELL_INIT
	return ; someone else is using the LCD, do not run the shell.
SHELL_UPDATE_CHECK nop
	banksel btn_pressed
	movf btn_pressed,F
	btfss STATUS,Z
	goto SHELL_BUTTON_PRESSED
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
	movlw BACKSPACE_CHAR_KEYBOARD ; is the character a backspace, if so remove character
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
DO_STUFF_IN_SHELL movlw 0x0 ; replace with null character and send to RUN_COMMAND
	movwf INDF
	goto READ_COMMAND	
SHELL_NO_CR movlw DEFAULT_SHELL_CHAR
	movwf INDF
	goto SHELL_UPDATE_DISPLAY
SHELL_BUTTON_PRESSED movlw low lcd_buffer
	movwf FSR
	movlw '>'
	movwf INDF
	movlw SHELL_ID
	call PUSH_STACK
	call BANK_0
	call WRITE_BUFFER
	banksel usart_addr
	movf usart_addr,W
	call PUSH_STACK
	call MORSE_CODE_TREE
	banksel usart_addr
	movf usart_addr,W
	banksel lcd_pointer
	movwf FSR
	movlw BACKSPACE_CHAR ; is the character a backspace, if so remove character
	xorwf INDF,W
	btfsc STATUS,Z
	goto SHELL_BACKSPACE
	movlw BACKSPACE_CHAR_KEYBOARD ; is the character a backspace, if so remove character
	xorwf INDF,W
	btfsc STATUS,Z
	goto SHELL_BACKSPACE
	movlw LF
	xorwf INDF,W
	btfsc STATUS,Z
	goto DO_STUFF_IN_SHELL
	goto SHELL_ADVANCE_UPDATE


READ_COMMAND movlw 1+low lcd_buffer ; plus one for the ?character
	movwf FSR
	banksel argv_head;initialize argv (zero if no arguments)
	clrf argv_head
	clrf argv_tail
	banksel crc
	clrf crc
READ_COMMAND_LOOP movf INDF,F ;Parse command characters until a null character is reached.
	btfsc STATUS,Z
	goto HAVE_COMMAND
	movlw ' '
	xorwf INDF,W
	btfsc STATUS,Z
	goto PARSE_ARGS
	movf INDF,W
	call PUSH_STACK
	call CALCULATE_CRC
	incf FSR,F
	goto READ_COMMAND_LOOP
PARSE_ARGS movf FSR,W
	call PUSH_STACK
	call GET_NEXT_ARG
	goto HAVE_COMMAND
HAVE_COMMAND call SHELL_INIT
	movlw EXEC_CRC
	banksel crc
	xorwf crc,W
	btfsc STATUS,Z
	goto EXEC
	movlw TIME_CRC
	xorwf crc,W
	btfsc STATUS,Z
	goto RUN_DISPLAY_TIME
	movlw SET_TIME_CRC
	xorwf crc,W
	btfsc STATUS,Z
	goto SHELL_SET_TIME
	goto EXEC ; ELSE!!!
RUN_DISPLAY_TIME movlw TIME_ID
	banksel lcd_owner 
	movwf lcd_owner
	goto DISPLAY_TIME
SHELL_SET_TIME call BANK_0
	clrf minutes
	clrf hours
	banksel argv_head
	movf argv_head,W
	btfsc STATUS,Z
	goto SHELL_SET_TIME_NULL_POINTER
	movwf FSR
	call BANK_0 ; args are in bank 0 (lcd_buffer)
	movlw 0x30
	subwf INDF,W
	movwf hours
	banksel argv_head
	movf argv_tail,W
	xorwf FSR,W
	btfsc STATUS,Z
	goto SHELL_SET_TIME_MINUTES
	movf hours,W
	call PUSH_STACK
	call MULT_BYTE_10
	movwf hours
	incf FSR,F
	movlw 0x30
	subwf INDF,W
	call BANK_0
	addwf hours
SHELL_SET_TIME_MINUTES 
	banksel argv_tail
	movf argv_tail,W
	call PUSH_STACK
	call GET_NEXT_ARG
	banksel argv_head
	movf argv_head,W
	btfsc STATUS,Z
	goto SHELL_SET_TIME_NULL_POINTER ; no minutes supplied :-/
	movwf FSR
	call BANK_0
	movf INDF,W
	sublw 0x30
	movwf minutes
	banksel argv_tail
	movf FSR,W
	xorwf argv_tail
	btfsc STATUS,Z
	goto SHELL_SET_TIME_BANK_0_RTN
	call BANK_0
	movf minutes,W
	call PUSH_STACK
	call MULT_BYTE_10
	movwf minutes
	incf FSR,F
	movf INDF,W
	sublw 0x30
	addwf minutes
	return
SHELL_SET_TIME_NULL_POINTER 
SHELL_SET_TIME_BANK_0_RTN goto BANK_0

GET_NEXT_ARG call POP_STACK
	movwf FSR
	banksel argv_head
	clrf argv_head
	clrf argv_tail
	call BANK_0
GET_NEXT_ARG_LOOP incf FSR,F ; iterate through characters in shell command after the first word
	movf INDF,F ;place the first non-white space address in argv_head
	btfsc STATUS,Z ; and the last address in argv_tail
	return
	movlw ' '
	xorwf INDF,W
	btfsc STATUS,Z
	goto GET_NEXT_ARG_LOOP
	movf FSR,W
	banksel argv_head
	movf argv_head,F
	btfsc STATUS,Z
	movwf argv_head
	movwf argv_tail
	call BANK_0
	goto GET_NEXT_ARG_LOOP
	endm