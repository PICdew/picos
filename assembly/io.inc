;Waits for the specified "return" button to be pressed.
;While that button is being waited on, a tree is followed
;using the other buttons, where one represents dit
;and the other represents dat of morse code.
;
;return_btn, dit and dat should be of the bit number within
;btn_pressed for the corresponding button
MORSE_CODE_TREE_MAC macro return_btn,dit,dat,btn_pressed,DIT_SOUND,DAT_SOUND,PUSH_STACK,SWAP_STACK
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
	call DIT_SOUND
	goto MC_REFRESH
MC_DAT bcf btn_pressed,dat
	call POP_STACK
	movwf INDF
	incf INDF,F
	rlf INDF,W
	call PUSH_STACK
	call W_TO_MORSE
	movwf INDF
	call DAT_SOUND
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


SOUND_MAC macro Delay255,Delay100,Delay50,temp

MUTE_SOUND call SAVE_BANK
	call BANK_0
	clrf CCP1CON
	goto RESET_BANK
	
TONE_440 call SAVE_BANK
	call BANK_0
	movlw 0x8d
	movwf PR2
	clrf CCPR1L
	bcf TRISC,5
	movlw b'111'
	movwf T2CON
	movlw 0x3c
	movwf CCP1CON
	movlw 0x46
	movwf CCPR1L
	goto RESET_BANK

DIT_SOUND call TONE_440
	call Delay100
TONE_END call MUTE_SOUND
	goto Delay50

DAT_SOUND call TONE_440
	call Delay100
	call Delay100
	call Delay100
	goto TONE_END

MORSE_SOUND movlw HIGH MORSE_SOUND_TABLE
	movwf PCLATH
	call POP_STACK
	movwf temp
	movlw 'A'
	subwf temp,W
	addlw low MORSE_SOUND_TABLE
	btfsc STATUS,C
	incf PCLATH,F ; long program o.O
	movwf PCL;USES RUN_COMMAND_TABLE as a lookup table.
MORSE_SOUND_TABLE goto A_SOUND
	goto B_SOUND
	goto C_SOUND
	goto D_SOUND
	goto DIT_SOUND;E
	goto F_SOUND
	goto G_SOUND
	goto H_SOUND
	goto I_SOUND
	goto J_SOUND
	goto K_SOUND
	goto L_SOUND
	goto M_SOUND
	goto N_SOUND
	goto O_SOUND
	goto P_SOUND
	goto Q_SOUND
	goto R_SOUND
	goto S_SOUND
	goto DAT_SOUND;T
	goto U_SOUND
	goto V_SOUND
	goto W_SOUND
	goto X_SOUND
	goto Y_SOUND
	goto Z_SOUND	

I_SOUND call DIT_SOUND
	goto DIT_SOUND
A_SOUND call DIT_SOUND
	goto DAT_SOUND
N_SOUND call DAT_SOUND
	goto DIT_SOUND
M_SOUND call DAT_SOUND
	goto DAT_SOUND
S_SOUND call DIT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
U_SOUND call DIT_SOUND
	call DIT_SOUND
	goto DAT_SOUND
R_SOUND call DIT_SOUND
	call DAT_SOUND
	goto DIT_SOUND
W_SOUND call DIT_SOUND
	call DAT_SOUND
	goto DAT_SOUND
D_SOUND call DAT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
K_SOUND call DAT_SOUND
	call DIT_SOUND
	goto DAT_SOUND
G_SOUND call DAT_SOUND
	call DAT_SOUND
	goto DIT_SOUND
O_SOUND call DAT_SOUND
	call DAT_SOUND
	goto DAT_SOUND
H_SOUND call DIT_SOUND
	call DIT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
V_SOUND call DIT_SOUND
	call DIT_SOUND
	call DIT_SOUND
	goto DAT_SOUND
F_SOUND call DIT_SOUND
	call DIT_SOUND
	call DAT_SOUND
	goto DIT_SOUND
L_SOUND call DIT_SOUND
	call DAT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
P_SOUND call DIT_SOUND
	call DAT_SOUND
	call DAT_SOUND
	goto DIT_SOUND
J_SOUND call DIT_SOUND
	call DAT_SOUND
	call DAT_SOUND
	goto DAT_SOUND		
B_SOUND call DAT_SOUND
	call DIT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
X_SOUND call DAT_SOUND
	call DIT_SOUND
	call DIT_SOUND
	goto DAT_SOUND
C_SOUND call DAT_SOUND
	call DIT_SOUND
	call DAT_SOUND
	goto DIT_SOUND
Y_SOUND call DAT_SOUND
	call DIT_SOUND
	call DAT_SOUND
	goto DAT_SOUND
Z_SOUND call DAT_SOUND
	call DAT_SOUND
	call DIT_SOUND
	goto DIT_SOUND
Q_SOUND call DAT_SOUND
	call DAT_SOUND
	call DIT_SOUND
	goto DAT_SOUND

	endm