;Waits for the specified "return" button to be pressed.
;While that button is being waited on, a tree is followed
;using the other buttons, where one represents dit
;and the other represents dat of morse code.
;
;return_btn, dit and dat should be of the bit number within
;btn_pressed for the corresponding button
MORSE_CODE_TREE_MAC macro return_btn,dit,dat,result_addr,btn_pressed,PUSH_STACK
MORSE_CODE_TREE movlw '?'
	movwf result_addr
	movlw 0
	call PUSH_STACK
MC_LOOP	btfsc btn_pressed,return_btn
	goto MC_RTN
	btfsc btn_pressed,dit
	goto MC_DIT
	btfsc btn_pressed,dat
	goto MC_DAT
	goto MC_LOOP
MC_DIT bcf btn_pressed,dit
	call POP_STACK
	movwf result_addr
	rlf result_addr,F
	incf result_addr,W
	call PUSH_STACK
	call W_TO_MORSE
	movwf result_addr
	goto MC_LOOP
MC_DAT bcf btn_pressed,dat
	call POP_STACK
	movwf result_addr
	incf result_addr,F
	rlf result_addr,W
	call PUSH_STACK
	call W_TO_MORSE
	movwf result_addr
	goto MC_LOOP
MC_RTN bcf btn_pressed,return_btn
	return

W_TO_MORSE movlw HIGH MORSE_TABLE
	movwf PCLATH
	call PEEK_STACK
	addlw low MORSE_TABLE
	btfsc STATUS,C
	incf PCLATH
	movwf PCL
MORSE_TABLE dt "?etianmsurwdkgohvf?l?pjbxcyzq??54?3???2??+????16=/?????7???8?90"	endm