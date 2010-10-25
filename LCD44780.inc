#define LCD_LINE_SIZE 0x8

LCD_ROUTINES_MAC macro count,count1,counta,countb,tmp1,tmp2,templcd,templcd2,MAIN_LOOP
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
	goto END_OF_DISPLAY
END_OF_DISPLAY goto MAIN_LOOP
STARTUP_TOP		addwf	PCL, f
	dt "PicOS vs",0x0
STARTUP_BOTTOM		ADDWF   PCL, f
	dt "READY!!!",0x00

DISPLAY_VERSION movlw KERNEL_MAJOR_VERSION
	call HEX_TO_CHAR
	call WRITE_CHAR
	movlw '.'
	call WRITE_CHAR
	movlw KERNEL_MINOR_VERSION + 1
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

HEX_TO_CHAR  	ADDWF   PCL       , f
            	dt "0123456789ABCDEF"
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
	incf templcd
	decfsz templcd2,F
	goto FILL_BUFFER_LOOP
	endm

WRITE_BUFFER_MAC macro lcd_buffer,count,WRITE_CHAR,LCD_TOP,LCD_BOTTOM
WRITE_BUFFER call LCD_TOP
	movlw low lcd_buffer
	movwf FSR
	movlw 0x8
	movwf count
WRITE_BUFFER_LOOP movf INDF,W
	call WRITE_CHAR
	incf FSR,F
	decfsz count,F
	goto WRITE_BUFFER_LOOP
	call LCD_BOTTOM
	movlw 0x8
	movwf count
WRITE_BUFFER_LOOP2 movf INDF,W
	call WRITE_CHAR
	incf FSR,F
	decfsz count,F
	goto WRITE_BUFFER_LOOP2
	endm

