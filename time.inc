
ISR_INCREMENT_TIME_OF_DAY_MAC macro timer_reg,myStatus,alarmHours,hours,alarmMinutes,minutes,INC_MINUTES,END_OF_INTERRUPT
ISR_INCREMENT_TIME_OF_DAY goto INC_MINUTES
ISR_REST_TMR movlw RESET_CLOCK_VALUE
	movwf timer_reg;reset timer value
	bcf CLOCK_TIMER_FLAG
	endm

;Provides SHOW_CLOCK, SHOW_TIME, SHOW_DATE
DISPLAY_TIME_MAC macro myStatus,indicator,dipControl,leftDisplay,rightDisplay,CREATE_DISPLAY
SHOW_CLOCK call SHOW_TIME
	bcf myStatus,0
	bcf myStatus,1
	movlw DEFAULT_DECIMAL_VALUE;light appropriate indicator
	btfsc myStatus,3
	movlw LEFT_DECIMAL_VALUE;left most decimalpoint
	movwf indicator
	;check for display type
	btfsc dipControl,DISPLAY_DATE_BIT
	call SHOW_DATE
	btfsc dipControl,DISPLAY_ALARM_BIT
	nop;fill this in
	return
	;
SHOW_DATE movlw 0x88
	movwf indicator
	movf dateDay,W
	movwf rightDisplay
	movf dateMonth,W
	movwf leftDisplay
	goto CREATE_DISPLAY
	;
SHOW_ALARM movlw 0x88
	movwf indicator
	movf alarmMinutes,W
	movwf rightDisplay
	movf alarmHours,W
	movwf leftDisplay
	goto CREATE_DISPLAY
	;
SHOW_TIME movlw 0x88
	movwf indicator
	movf minutes,W
	movwf rightDisplay
	movf hours,W
	movwf leftDisplay
	goto CREATE_DISPLAY
	;
	endm

;Provides INC_MINUTES, INC_HOURS, INC_DAYS, INC_MONTH
INCREMENT_TIME_MAC macro dateDay,dateMonth,minutes,hours,ISR_REST_TMR,NUMBER_OF_DAYS
INC_MINUTES incf minutes,F
	movlw .60
	xorwf minutes,W
	btfsc STATUS,Z
	goto INC_HOURS
	goto ISR_REST_TMR
	;
INC_HOURS clrf minutes
	incf hours,F
	movlw .24
	xorwf hours,W
	btfss STATUS,Z
	goto ISR_REST_TMR
	clrf hours
	goto INC_DAYS
	;
INC_DAYS movf dateMonth,W
	call NUMBER_OF_DAYS
	xorwf dateDay,W
	btfsc STATUS,Z
	goto INC_MONTH
	incf dateDay,F
	goto ISR_REST_TMR
	;
INC_MONTH movlw .1
	movwf dateDay
	incf dateMonth,F
	movlw .13
	xorwf dateMonth,W
	btfss STATUS,Z
	goto ISR_REST_TMR
	movlw .1
	movwf dateMonth
	goto ISR_REST_TMR
	;
	endm

TOGGLE_ALARM_MAC macro myStatus,ALARM_FLAG_TOGGLE
TOGGLE_ALARM movlw ALARM_FLAG_TOGGLE
	xorwf myStatus,F;toggle bit alarm flag
	return
	endm
;
NUMBER_OF_DAYS_MAC macro pclTemp
NUMBER_OF_DAYS movwf pclTemp
	movlw HIGH NUMBER_OF_DAYS_TABLE
	movwf PCLATH
	movf pclTemp,W
	addwf PCL,F
NUMBER_OF_DAYS_TABLE retlw .30;!?!?!?!?
	retlw .31;jan
	retlw .28;feb (leap year???)
	retlw .31;mar
	retlw .30;apr
	retlw .31;may
	retlw .30;june
	retlw .31;july
	retlw .31;august
	retlw .30;sept
	retlw .31;oct
	retlw .30;nov
	retlw .31;dec
	endm
