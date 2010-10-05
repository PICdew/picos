	;
	;clock software 1.0
	;Author: David Coss
	;Copyright: Free to use with attribution to the Author. 
	;
	; See bottom for description of changes per version.
	;
#include "header.inc"
#include "mem.inc"
#include "kernel.inc"
#include "../../piclang/piclang.inc"
#include "bin_7seg.inc"
#include "stack.inc"
	org 0x00
	goto INIT
	;
	org 0x04
	START_INTERRUPT saveW
	call INC_MINUTES
	movf resetTMR0,W
	movwf TMR0;reset timer value
	bcf controlPort,1
	btfss myStatus,3
	goto END_OF_INTERRUPT
	movf alarmHours,W
	subwf hours,W
	btfss STATUS,Z
	goto END_OF_INTERRUPT
	movf alarmMinutes,W
	subwf minutes,W
	btfsc STATUS,Z
	bsf controlPort,1
END_OF_INTERRUPT nop
	FINISH_INTERRUPT saveW
	retfie
INIT INIT_KERNEL_MAC errorByte 
	INIT_MEMORY_MAC endOfMemory;init stack
	INIT_STACK_MAC stackHead,stackPtr
	;
	;init display
	INIT_DISPLAY_MAC firstDisplay,lastDisplay,indicator
	movlw INITIAL_TIMER_VALUE
	movwf resetTMR0;
	;
	;initialize ports
	movlw b'10111'
	movwf myStatus;default hour, 7-seg, right display, alarm off, use hex
	;bit 0 = output Hour/~output Minute. bit 1 = ~7-seg/binary display. bit 2 = right 7-seg/~left 7-seg. bit 3 = alarm on/~off
	;
	bcf STATUS,RP0
	bcf STATUS,RP1;
	clrf outport
	clrf controlPort
	clrf dipControl
	bsf STATUS,RP0;bank 1
	movlw 0x0; all output
	movwf outportTRIS; Hours/~Minutes(7bit), Binary/~7-seg(6bit), When 7-seg is on bit 4 is ~minutes and bit 7 is ~hour
	movlw 0x6;
	movwf ADCON1;SET bit 0 of porta to be digital (not analog :-/ )
	movlw b'00010001';TIMER MUST BE INPUT!!!
	movwf controlPortTRIS;bit 0 = Program Mode On, bit 1 = external alarm trigger, 4 = timer0 clock
	movlw b'11111111';Oh, yeah. dip switch keyboard
	movwf dipControlTRIS;bit 7 = trigger for dip switch commands.
	bsf OPTION_REG,5;turn on timer0
	bcf OPTION_REG,4;time on leading edge of T0CKI
	bcf STATUS,RP0
	;
	bcf STATUS,0       ; Clear carry bit
	bcf STATUS,2       ; Clear zero flag
	bcf STATUS,1       ;
	bsf INTCON,5       ; Enable timer0 interrupt
	bcf INTCON,2       ; Clear interrupt flag
	bsf INTCON,7       ; Enable global interrupt
	movf resetTMR0,W
	movwf TMR0; Count from c4 to overflow (60secs) then interrupt
	;
	movlw 0x40
	movwf eepromSize
	movlw .0
	movwf output
	movlw .29
	movwf dateDay
	movlw 0x6
	movwf dateMonth
	movlw 0x00;start at midnight
	movwf hours
	movwf minutes
	movwf alarmMinutes
	movwf alarmHours;off by default. cf mystatus.
	bcf controlPort,1
	movlw .0
	movwf counter
	;
MAIN_LOOP call showclock
	MAIN_PROCESS_MAC CREATE_DISPLAY,DISPLAY_ME,controlPort,PROGRAM_MODE,MAIN_LOOP;generic process loop from kernel.asm
	;
PROGRAM_MODE INIT_STACK_MAC stackHead,stackPtr
	movlw 0x0
	call PUSH_STACK
	PROGRAM_LOOP_MAC dipControl,INTPUT_BIT,instruction,RUN_PROGRAM,accumulator,exchange,PROGRAM_MAIN_FORK,WRITE_EEPROM,READ_EEPROM,PUSH_STACK,POP_STACK
	;
	;
;Subroutines
#include "program_subroutines.asm"
	;macro calls
	CREATE_DISPLAY_MAC myStatus,minutes,hours,MAKE_PACKET,binaryMinute,binaryHours,controlPort,rightDisplay,SEG_VALUES,output,LOAD_PACKET,indicator,hexToOctal 
	LOAD_PACKET_MAC firstDisplay,currSeg,MAKE_PACKET,output,indicator
	MAKE_PACKET_MAC tmp,myStatus 
	SEG_VALUES_MAC pclTemp
	DISPLAY_ME_MAC firstDisplay,outport,binaryHours,lastDisplay 	
	PUSH_STACK_MAC stackTemp,stackPtr
	POP_STACK_MAC stackPtr
	GET_ARG_MAC programCounter,READ_EEPROM,PUSH_STACK
	WRITE_EEPROM_MAC stackPtr
	READ_EEPROM_MAC stackPtr
	RUN_PROGRAM_MAC stackHead,stackPtr,programCounter,READ_EEPROM,instruction,EOP,RUN_COMMAND
	RUN_COMMAND_MAC instruction,programCounter,RUN_COMMAND_TABLE
	PICLANG_COMMAND_SET_MAC GET_ARG,POP_STACK,PUSH_STACK,accumulator,exchange,instruction,END_OF_FUNCTION,WRITE_EEPROM

	;
	;clock stuff
showclock call SHOW_TIME
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
INC_MINUTES incf minutes,F
	movlw .60
	xorwf minutes,W
	btfsc STATUS,Z
	call INC_HOURS
	incf totalMinutesLOW,F
	btfsc STATUS,C
	incf totalMinutesMIDDLE,F
	btfsc STATUS,C
	incf totalMinutesHIGH,F
	return
	;
INC_HOURS clrf minutes
	incf hours,F
	movlw .24
	xorwf hours,W
	btfss STATUS,Z
	return
	clrf hours
	goto INC_DAYS
	;
INC_DAYS movf dateMonth,W
	call NUMBER_OF_DAYS
	xorwf dateDay,W
	btfsc STATUS,Z
	goto INC_MONTH
	incf dateDay,F
	return
	;
INC_MONTH movlw .1
	movwf dateDay
	incf dateMonth,F
	movlw .13
	xorwf dateMonth,W
	btfss STATUS,Z
	return
	movlw .1
	movwf dateMonth
	;
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
	;
TOGGLE_ALARM movlw ALARM_FLAG_TOGGLE
	xorwf myStatus,F;toggle bit alarm flag
	return
;
;
NUMBER_OF_DAYS movwf pclTemp
	movlw HIGH SEG_VALUES
	movwf PCLATH
	movf pclTemp,W
	addwf PCL,F
	retlw .30;!?!?!?!?
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
;

	;
	END

	;1.1: Added multiple opcodes.
	;1.0:	added defines. Added stack. stackPtr will always
	;		contain the "top" of the stack. Therefore,
	;		popping the stack means:
	;			movf stackPtr,W
	;			movwf FSR
	;			decf stackPtr,F
	;		INDF will then contain the data that has
	;		been popped. Pushing into the stack is
	;		done by:
	;			incf stackPtr,F
	;			movwf INDF
	;			
	;0.8:	Added Date!
	;0.7:	Added alarm clock function. Also, I changed
	;		the way time is set. Now, it uses FSR to
	;		set the hours and minutes one at a time.
	;		This also allows me to use the same routine
	;		for the alarm and the time and it gives me more
	;		command options (and saving program memory :-D )
	;
	;0.6.0: Completely changed setup to *store* binary & 
	;		7-seg packets so that more time can be spent
	;		lighting them.
	;0.5.1: Fixed rotation error in switching between hex
	;		and octal outputs
	;0.5.0:	Added controls to set time using PORTB connected
	;		to a 8-bit dip switch.
	;0.4.4: Fixed the left 7-seg display for octal output.
	;0.4.3: Added the switch to change PORTA from analog to 
	;		digital input.
	;0.4.2: Fixed 7-seg ata table values. Changed start time 
	;       to 0:00. Fixed a mistake which prevented the hex
	;		display flag from being set with minutes. Set bit
	;		0 of PORTA to input (hex/~octal flag)
	;0.4.1: Added comments to understand which bits control 
	;		specific LED's in the output. Set TMR0 to input
	;		and all other PORTA bits to output. Time starts
	;		at 5:05am so that I can test different bits and
	;		output at startup.
