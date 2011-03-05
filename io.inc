;Waits for the specified "return" button to be pressed.
;While that button is being waited on, a tree is followed
;using the other buttons, where one represents dit
;and the other represents dat of morse code.
;
;return_btn, dit and dat should be of the bit number within
;btn_pressed for the corresponding button
MORSE_CODE_TREE_MAC macro return_btn,dit,dat,btn_pressed,PUSH_STACK,SWAP_STACK
MORSE_CODE_TREE movf FSR,W
	call SWAP_STACK
	movwf FSR
	movlw '?'
	movlw 0
	call PUSH_STACK
	banksel btn_pressed
MC_LOOP	btfsc btn_pressed,return_btn
	goto MC_RTN
	btfsc btn_pressed,dit
	goto MC_DIT
	btfsc btn_pressed,dat
	goto MC_DAT
	goto MC_LOOP
MC_DIT bcf btn_pressed,dit
	call POP_STACK
	movwf INDF
	rlf INDF,F
	incf INDF,W
	call PUSH_STACK
	call W_TO_MORSE
	movwf INDF
	goto MC_REFRESH
MC_DAT bcf btn_pressed,dat
	call POP_STACK
	movwf INDF
	incf INDF,F
	rlf INDF,W
	call PUSH_STACK
	call W_TO_MORSE
	movwf INDF
	goto MC_REFRESH
MC_RTN bcf btn_pressed,return_btn
	call POP_STACK ; morse counter
	call POP_STACK ; old fsr
	movwf FSR
	goto BANK_0
MC_REFRESH movf FSR,W
	call PUSH_STACK
	movlw SHELL_ID
	call PUSH_STACK
	call WRITE_BUFFER ; pops stack
	call POP_STACK
	movwf FSR
	banksel btn_pressed
	goto MC_LOOP ; continue MORSE CODE LOOP

W_TO_MORSE movlw HIGH MORSE_TABLE
	movwf PCLATH
	call PEEK_STACK
	addlw low MORSE_TABLE
	btfsc STATUS,C
	incf PCLATH
	movwf PCL
MORSE_TABLE dt "?etianmsurwdkgohvf",0x7F,"l",0xa,"pjbxcyzq??54?3???2??+????16=/?????7???8?90"
	endm

DEBOUNCE_BUTTONS_MAC macro btn_state,btn_buffer,btn_pressed
;; btn routine
DEBOUNCE_BUTTONS call GET_BUTTON_STATE
	call PUSH_STACK
	banksel btn_state
	xorwf btn_buffer,W
	btfsc STATUS,Z
	goto DEBOUNCE_SAME_STATE
	goto DEBOUNCE_CHANGE_STATE
DEBOUNCE_SAME_STATE incf btn_counter,F
	btfsc STATUS,Z
	goto UPDATE_BTN_STATE
	goto END_OF_DEBOUNCE1
UPDATE_BTN_STATE comf btn_buffer,W
	andwf btn_state,W
	iorwf btn_pressed,F ; ior because I want a system monitor or user to clear the pressed bit when they've used the fact that the button's been pressed.
	movf btn_buffer,W
	movwf btn_state
	goto END_OF_DEBOUNCE1
DEBOUNCE_CHANGE_STATE call POP_STACK
	movwf btn_buffer
	movlw 0xff-0x20
	movwf btn_counter;clrf btn_counter
	goto END_OF_DEBOUNCE
END_OF_DEBOUNCE1 call POP_STACK
END_OF_DEBOUNCE goto INTERRUPT_CHECKED_TIME;return
	endm
