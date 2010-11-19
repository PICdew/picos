;for a list of changes see bottom of file.
#define LCD_LINE_SIZE 0x8

LCD_ROUTINES_MAC macro count,count1,counta,countb,tmp1,tmp2,templcd,templcd2,pclTemp,lcd_pointer,lcd_pointer,MAIN_LOOP
STARTUP_MESSAGE clrf count			;set counter register to zero
STARTUP1 movf	count, w		;put counter value in W
		call	STARTUP_TOP;get a character from the text table
		xorlw	0x00			;is it a zero?
		btfsc	STATUS, Z
		goto	NEXT_STARTUP_LINE
		call	WRITE_CHAR
		call	Delay255
		incf	count, f
		goto	STARTUP1

NEXT_STARTUP_LINE call	LCD_BOTTOM		;move to 2nd row, first column
	call DISPLAY_VERSION
	movlw 4
	movwf templcd
	call Delay255
	decfsz templcd,F
	goto $-2	
	goto END_OF_DISPLAY
END_OF_DISPLAY goto MAIN_LOOP
STARTUP_TOP	movwf pclTemp
	movlw HIGH STARTUP_TOP_TABLE
	movwf PCLATH
	movf pclTemp,W
	addlw low STARTUP_TOP_TABLE
	btfsc STATUS,C
	incf PCLATH,F
	movwf PCL
STARTUP_TOP_TABLE dt "PicOS vs",0x0

DISPLAY_VERSION movlw KERNEL_MAJOR_VERSION
	call HEX_TO_CHAR
	call WRITE_CHAR
	movlw '.'
	call WRITE_CHAR
	movlw KERNEL_MINOR_VERSION
	call HEX_TO_CHAR
	call WRITE_CHAR
	movlw '.'
	call WRITE_CHAR
	movlw KERNEL_REVISION
	call HEX_TO_CHAR
	goto WRITE_CHAR
	
;LCD routines

;Initialise LCD
INIT_LCD movlw	0x20;only use four bits (save the others)
	call	LCD_CMD
	movlw	0x28;Set 2 lines and display shift
	call	LCD_CMD
	movlw	0x06;Set display mode
	call	LCD_CMD
	movlw	0x0d;Turn display on/off
	call	LCD_CMD
	call	CLEAR_DISPLAY			;clear display
	movlw low lcd_buffer
	movwf lcd_pointer
	return

;Commands
LCD_CMD	movwf	templcd
		swapf	templcd,	w	;send upper nibble
		andlw	0x0f			;clear upper 4 bits of W
		movwf	LCD_PORT
		bcf	CMD_PORT, LCD_RS	;RS line to 0
		call	TOGGLE_ENABLE			;Pulse the E line high
		movf	templcd,	w	;send lower nibble
		andlw	0x0f			;clear upper 4 bits of W
		movwf	LCD_PORT
		bcf	CMD_PORT, LCD_RS	;RS line to 0
		call	TOGGLE_ENABLE			;Pulse the E line high
		call 	Delay5
		return

DEC_TO_CHAR	addlw	0x30
WRITE_CHAR	movwf	templcd
		swapf	templcd,	w	;send upper nibble
		andlw	0x0f			;clear upper 4 bits of W
		movwf	LCD_PORT
		bsf	CMD_PORT, LCD_RS	;RS line to 1
		call	TOGGLE_ENABLE			;Pulse the E line high
		movf	templcd,	w	;send lower nibble
		andlw	0x0f			;clear upper 4 bits of W
		movwf	LCD_PORT
		bsf	CMD_PORT, LCD_RS	;RS line to 1
		call	TOGGLE_ENABLE			;Pulse the E line high
		call 	Delay5
		return

LCD_TOP	movlw	0x80			;move to 1st row, first column
		call	LCD_CMD
		return

LCD_BOTTOM	movlw	0xc0			;move to 2nd row, first column
		call	LCD_CMD
		return

LCD_TOPW	addlw	0x80			;move to 1st row, column W
		call	LCD_CMD
		return

LCD_BOTTOMW	addlw	0xc0			;move to 2nd row, column W
		call	LCD_CMD
		return

LCD_CurOn	movlw	0x0d			;Set display on/off and cursor command
		call	LCD_CMD
		return

LCD_CurOff	movlw	0x0c			;Set display on/off and cursor command
		call	LCD_CMD
		return

CLEAR_DISPLAY		movlw	0x01			;Clear display
		call	LCD_CMD
		retlw	0x00

LCD_HEX		movwf	tmp1
		swapf	tmp1,	w
		andlw	0x0f
		call	HEX_TO_CHAR
		call	WRITE_CHAR
		movf	tmp1, w
		andlw	0x0f
		call	HEX_TO_CHAR
		call	WRITE_CHAR
		retlw	0x00

Delay255	movlw	0xff		;delay 255 mS
		goto	d0
Delay100	movlw	d'100'		;delay 100mS
		goto	d0
Delay50		movlw	d'50'		;delay 50mS
		goto	d0
Delay20		movlw	d'20'		;delay 20mS
		goto	d0
Delay5		movlw	0x05		;delay 5.000 ms (4 MHz clock)
d0		movwf	count1
d1		movlw	0xC7			;delay 1mS
		movwf	counta
		movlw	0x01
		movwf	countb
Delay_0
		decfsz	counta, f
		goto	$+2
		decfsz	countb, f
		goto	Delay_0

		decfsz	count1	,f
		goto	d1
		retlw	0x00

TOGGLE_ENABLE		bsf	CMD_PORT, LCD_E
		nop
		bcf	CMD_PORT, LCD_E
		retlw	0x00

;end of LCD routines

;Ignores upper nibble
HEX_TO_CHAR andlw 0xf
	movwf pclTemp
	movlw HIGH HEX_TO_CHAR_TABLE
	movwf PCLATH
	movf pclTemp,W
	ADDWF   PCL       , f
HEX_TO_CHAR_TABLE dt "0123456789ABCDEF"
	endm

FILL_BUFFER_MAC macro lcd_buffer,templcd,templcd2
FILL_BUFFER movlw low lcd_buffer
	movwf FSR
	movlw 0
	movwf templcd
	movlw LCD_LINE_SIZE * 2
	movwf templcd2
FILL_BUFFER_LOOP movf templcd,W
	call HEX_TO_CHAR
	movwf INDF
	incf FSR,F
	incf templcd,F
	decfsz templcd2,F
	goto FILL_BUFFER_LOOP
	endm

CLEAR_LCD_BUFFER_MAC macro lcd_buffer,lcd_pointer,templcd
CLEAR_LCD_BUFFER movlw low lcd_buffer
	movwf lcd_pointer
	movwf FSR
	movlw LCD_LINE_SIZE * 2
	movwf templcd
	movlw ' '
	movwf INDF
	incf FSR,F
	decfsz templcd,F
	goto $-3
	return
	endm

;writes the buffer. The requester's address(UID) (first PCB byte) must be 
;at the top of the stack. If that value is null, the owner
;is assumed to be a kernel process. If the requester's id is
;not equal to the owner's id, the routine returns with a 
;LCD_REQUEST_DENIED error
WRITE_BUFFER_MAC macro lcd_buffer,count,lcd_owner,errorByte,WRITE_CHAR,LCD_TOP,LCD_BOTTOM,POP_STACK,SAVE_BANK,RESET_BANK
CAN_UPDATE_DISPLAY call SAVE_BANK
	call POP_STACK
	banksel lcd_owner
	xorwf lcd_owner,W ; does the UID equal the owner? If so, return 0
	btfsc STATUS,Z
	retlw 0
	movlw 0
	xorwf lcd_owner,W ; if the UID does not equal the owner. Is the owner null? If so, give the display to the UID
	call RESET_BANK
	btfss STATUS,Z
	retlw LCD_REQUEST_DENIED
	retlw 0

WRITE_BUFFER call POP_STACK
	call PUSH_STACK
	call PUSH_STACK
	call CAN_UPDATE_DISPLAY
	xorlw 0
	btfss STATUS,Z
	goto WB_LCD_REQUEST_DENIED
	DO_WRITE_BUFFER nop
	banksel lcd_buffer
	call CLEAR_DISPLAY
	call LCD_TOP
	movlw low lcd_buffer
	movwf FSR
	movlw 0x8
	movwf count
WRITE_BUFFER_LOOP movlw 0
	xorwf INDF,W
	btfsc STATUS,Z
	goto $+6
	movf INDF,W
	call WRITE_CHAR
	incf FSR,F
	decfsz count,F
	goto WRITE_BUFFER_LOOP
	call LCD_BOTTOM
	movlw 0x8
	movwf count
	movlw 0x0
	xorwf INDF,W
	btfsc STATUS,Z
	incf FSR,F
WRITE_BUFFER_LOOP2 movlw 0
	xorwf INDF,W
	btfsc STATUS,Z
	goto END_OF_WRITE_BUFFER
	movf INDF,W
	call WRITE_CHAR
	incf FSR,F
	decfsz count,F
	goto WRITE_BUFFER_LOOP2
	goto END_OF_WRITE_BUFFER
WB_LCD_REQUEST_DENIED bcf STATUS,RP0
	bcf STATUS,RP1
	movlw LCD_REQUEST_DENIED
	movwf errorByte
END_OF_WRITE_BUFFER call POP_STACK
	banksel lcd_owner
	movwf lcd_owner
	banksel lcd_buffer
	return
	endm

;11/19/2010 -- Made HEX_TO_CHAR ignore the upper nibble. This will allow number to be sent to HEX_TO_CHAR without 
;		going to pass the end of the lookup table if the value is > 15