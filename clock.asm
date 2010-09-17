	;
	;clock software 1.0
	;Author: David Coss
	;Copyright: Free to use with attribution to the Author. 
	;
	; See bottom for description of changes per version.
	;
	
	list p=16f870
	include <p16f870.inc>
	radix hex
	
#define outport PORTC
#define outportTRIS TRISC
#define controlPort PORTA
#define controlPortTRIS TRISA
#define dipControl PORTB
#define dipControlTRIS TRISB
#define FIRST_DISPLAY_PTR 0x53
#define LAST_DISPLAY_PTR 0x54
#define DEFAULT_DECIMAL_VALUE 0x10
#define LEFT_DECIMAL_VALUE 0x10
#define RIGHT_DECIMAL_VALUE 0x8
#define MSN_RIGHT_DECIMAL_VALUE 0x18
#define DISPLAY_MEMORY_WIDTH 0x8
#define INITIAL_TIMER_VALUE 0xc4
#define TIME_ADDR 0x26
#define ALARM_ADDR 0x55
#define DATE_ADDR 0x5f
#define ACCUMULATOR_ADDR 0x61
#define EXHANGE_ADDR 0X62
#define STACK_HEAD_ADDR 0x63h
#define STACK_TAIL_ADDR 0x6Bh
#define STACK_LENGTH 8

;In the follwing bit definitions, the first word/phrase indicates
;the meaning of Logic 1 and the second word/phrase indicates
;the meaning of Logic 0
;
;myStatus bits
#define HOUR_MIN_BIT 0
#define BINARY_7SEG_BIT 1
;N stands for nibble
#define LSN_MSG_7SEG_BIT 2
#define ALARM_ON_OFF_BIT 3
#define ALARM_FLAG_TOGGLE 0x8
	
;DIP input port bits
#define INPUT_BIT 7

;control port bits
#define HEX_OCTAL_BIT 0
#define ALARM_TRIGGER_BIT 1
#define TIMER0_CLK 4
		
endOfNamedSpots EQU 20h
output EQU 21h
counter EQU 22h
tmp EQU 23h
myStatus EQU 24h
saveW EQU 25h
minutes EQU 26h
hours EQU 27h
currSeg EQU 28h
valueToLatch EQU 29h
totalMinutesLOW EQU 2Ah;total minutes since start of timer. why not? might come in handy later.
totalMinutesMIDDLE EQU 2Bh;Middle of a 3byte counter of total minutes
totalMinutesHIGH EQU 2Ch;end of total minutes segment. increment LOW, if carryover increment middle, if carryover increment high. if carryover... whoa!
setTimeTmp EQU 2Dh
resetTMR0 EQU 2Eh;TMR0's initial value
hexToOctal EQU 2Fh;memory spot for bit rotation
lastCommand EQU 30h
lsdMinute EQU 31h
msdMinute EQU 39h;7-seg packets for digits
lsdHour EQU 3Ah
msdHour EQU 42h
binaryMinute EQU 51h
binaryHours EQU 52h;binary packets
firstDisplay EQU 53h;lsd pointer
lastDisplay EQU 54h;binaryHours pointer
alarmMinutes EQU 55h;
alarmHours EQU 56h;Alarm Clock! cf myStatus
leftDisplay EQU 57h
rightDisplay EQU 58h;Display place holders to make create display more generic. to show time, load minutes/hours into it.
indicator EQU 59h;which decimal to light (tells a story)
timePointer EQU 5Ah
alarmPointer EQU 5Bh
datePointer EQU 5Ch
setTimeFSR EQU 5Dh
eepromSize EQU 5Eh
dateDay EQU 5Fh
dateMonth EQU 60h
accumulator EQU 61h
exchange EQU 62h
stackHead EQU 63h
stackTail EQU 6Bh
	org 0x00
	goto INIT
	;
	org 0x04
	movwf saveW;save w value
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
END_OF_INTERRUPT movf saveW,W
	bcf INTCON,2
	retfie
INIT movlw 0x60;last memoryspot
	movwf endOfNamedSpots;So I can keep track of when my "name space" ends
	;
	movlw FIRST_DISPLAY_PTR
	movwf firstDisplay;set display pointers
	movlw LAST_DISPLAY_PTR
	movwf lastDisplay
	movlw 0x2
	subwf lastDisplay,W
	movwf FSR;clear decimal points on 7-seg
	movlw DEFAULT_DECIMAL_VALUE
	movwf INDF
	movlw DISPLAY_MEMORY_WIDTH
	subwf FSR,F
	movlw DEFAULT_DECIMAL_VALUE
	movwf INDF
	movlw DISPLAY_MEMORY_WIDTH
	subwf FSR,F
	movlw DEFAULT_DECIMAL_VALUE
	movwf INDF
	movlw DISPLAY_MEMORY_WIDTH
	subwf FSR,F
	movlw DEFAULT_DECIMAL_VALUE
	movwf INDF
	;
	movlw INITIAL_TIMER_VALUE
	movwf resetTMR0;
	movlw b'0111'
	movwf myStatus;bit 0 = output Hour/~output Minute. bit 1 = ~7-seg/binary display. bit 2 = right 7-seg/~left 7-seg. bit 3 = alarm on/~off
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
	movwf controlPortTRIS;bit 0 = hex/~octal, bit 1 = external alarm trigger, 4 = timer0 clock
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
	movlw TIME_ADDR
	movwf timePointer
	movlw ALARM_ADDR
	movwf alarmPointer
	movlw DATE_ADD
	movwf datePointer
	movlw DEFAULT_DECIMAL_VALUE;by default light 2nd decimal (from left)
	movwf indicator
	movlw .29
	movwf dateDay;hehe
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
MAIN_LOOP movf minutes,W
	movwf rightDisplay
	movf hours,W
	movwf leftDisplay
	bcf myStatus,0
	bcf myStatus,1
	movlw DEFAULT_DECIMAL_VALUE;light appropriate indicator
	btfsc myStatus,3
	movlw LEFT_DECIMAL_VALUE;left most decimalpoint
	movwf indicator
	call CREATE_DISPLAY
	movlw 0xff
	movwf counter
DISPLAY_LOOP call DISPLAY_ME
	decfsz counter
	goto DISPLAY_LOOP
	clrwdt;WDT checks for run away code. If I get this far, I'm not running away
	btfsc dipControl,INPUT_BIT
	call PARSE_COMMAND
	goto MAIN_LOOP ; repeat....
	;
CREATE_DISPLAY bcf myStatus,HOUR_MIN_BIT ;; display minutes
	bsf myStatus,BINARY_7SEG_BIT;binary first
	bsf myStatus,LSN_MSG_7SEG_BIT;right first
	movf minutes,W;time *always* displayed in binary!
	call MAKE_PACKET
	movwf binaryMinute
	movlw 0x0f
	btfss controlPort,0
	movlw b'0111'
	andwf rightDisplay,W
	bcf myStatus,BINARY_7SEG_BIT;hex/octal is displayed first
	call SEG_VALUES
	movwf output
	movlw 0x0
	call LOAD_PACKET;loads 7-seg packet to memory to be displayed
	movf indicator,W;light decimal
	movwf INDF
	movlw 0xf0
	btfss controlPort,HEX_OCTAL_BIT
	movlw b'111000'
	andwf rightDisplay,W
	bcf STATUS,C
	movwf hexToOctal
	rrf hexToOctal,F
	rrf hexToOctal,F
	rrf hexToOctal,F
	btfsc controlPort,HEX_OCTAL_BIT
	rrf hexToOctal,F;swapf W,W
	movf hexToOctal,W
	call SEG_VALUES
	bcf myStatus,LSN_MSG_7SEG_BIT;left 7-seg
	movwf output
	movlw RIGHT_DECIMAL_VALUE
	call LOAD_PACKET
	movf indicator,W; light decimal
	movwf INDF
	;
	;display hours
	;
	bsf myStatus,HOUR_MIN_BIT;hours
	bsf myStatus,BINARY_7SEG_BIT;binary first
	bsf myStatus,LSN_MSG_7SEG_BIT;right first
	movf hours,W
	call MAKE_PACKET
	movwf binaryHours
	movlw 0xf
	btfss controlPort,HEX_OCTAL_BIT
	movlw b'0111'
	andwf leftDisplay,W
	bcf myStatus,1;hec/octal
	call SEG_VALUES
	movwf output
	movlw LEFT_DECIMAL_VALUE
	call LOAD_PACKET
	movf indicator,W;light decimal
	movwf INDF
	movlw 0xf0
	btfss controlPort,0
	movlw b'111000'
	andwf leftDisplay,W
	bcf STATUS,C
	movwf hexToOctal
	rrf hexToOctal,F
	rrf hexToOctal,F
	rrf hexToOctal,F
	btfsc controlPort,HEX_OCTAL_BIT
	rrf hexToOctal,F;swapf W,W
	movf hexToOctal,W
	call SEG_VALUES
	bcf myStatus, LSN_MSG_7SEG_BIT;left 7-seg
	movwf output
	movlw MSN_RIGHT_DECIMAL_VALUE
	call LOAD_PACKET		
	movf indicator,W;light decimal
	movwf INDF
	return
LOAD_PACKET addwf firstDisplay,W;digit offset
	movwf FSR
	movlw 0x7
	movwf currSeg
LOAD_PACKET_LOOP movf currSeg,W
	call MAKE_PACKET
	bcf STATUS,C
	rlf output,F
	btfss STATUS,C
	movf indicator,W;if that seg is dark just do a decimal
	movwf INDF
	incf FSR,F
	decfsz currSeg,F
	goto LOAD_PACKET_LOOP
	return
	;
MAKE_PACKET movwf tmp
	bcf tmp,7
	bcf tmp,6
	btfsc myStatus,HOUR_MIN_BIT;hours/~min
	bsf tmp,7
	btfsc myStatus,BINARY_7SEG_BIT;~7-seg/binary
	bsf tmp,6
	btfsc myStatus,BINARY_7SEG_BIT
	goto REST_OF_MAKE_PACKET
	bsf tmp,7
	bsf tmp,4
	btfsc myStatus,HOUR_MIN_BIT
	bcf tmp,7
	btfss myStatus,HOUR_MIN_BIT
	bcf tmp,4
REST_OF_MAKE_PACKET movlw 0x0
	btfss myStatus,LSN_MSG_7SEG_BIT;add offset for left 7-seg
	movlw RIGHT_DECIMAL_VALUE
	addwf tmp,F
	movf tmp,W
	return
	
SEG_VALUES addwf PCL,F
	retlw b'01111110';0
	retlw b'00000110';1
	retlw b'11011010';2
	retlw b'11001110';3
	retlw b'10100110';4
	retlw b'11101100';5
	retlw b'11111100';6
	retlw b'01000110';7
	retlw b'11111110';8
	retlw b'11100110';9
	retlw b'11110110';A
	retlw b'10111100';B
	retlw b'01111000';C
	retlw b'10011110';D
	retlw b'11111000';E
	retlw b'11110000';F
	retlw 0x1;decimal point
	;
NUMBER_OF_DAYS addwf PCL,F
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
INC_MINUTES incf minutes,F
	movlw .60
	xorwf minutes,W
	btfsc STATUS,Z
	call INC_HOURS
	incf totalMinutesLOW
	btfsc STATUS,C
	incf totalMinutesMIDDLE
	btfsc STATUS,C
	incf totalMinutesHIGH
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
	incf dateMonth
	movlw .13
	xorwf dateMonth,W
	btfss STATUS,Z
	return
	movlw .1
	movwf dateMonth
	;
PARSE_COMMAND movf dipControl,W	; !!! TO BE REPLACED BY ASSEMBLY BASED LANGUAGE
	movwf lastCommand
	bcf lastCommand,INPUT_BIT; input write flag is not needed.
	movlw 0x0
	subwf lastCommand,W
	btfsc STATUS,Z
	goto SET_CURRENT_TIME
	movlw 0x1
	subwf lastCommand,W
	btfsc STATUS,Z 
	goto SET_ALARM_TIME
	movlw .42
	subwf lastCommand,W
	btfsc STATUS,Z
	goto DO_COMMAND_0
	movlw 0x2
	subwf lastCommand,W
	btfsc STATUS,Z
	goto TOGGLE_ALARM
	movlw 0x3
	subwf lastCommand,W
	btfsc STATUS,Z
	goto SET_DATE
	movlw 0x4
	subwf lastCommand,W
	btfsc STATUS,Z
	goto SHOW_DATE
	return
	;
DISPLAY_ME movf firstDisplay,W
	movwf FSR
DISPLAY_ME_LOOP	movf binaryMinute,W
	movwf outport
	movf binaryHours,W
	movwf outport
	movf INDF,W
	movwf outport
	incf FSR,F
	movf lastDisplay,W
	subwf FSR,W
	btfss STATUS,Z
	goto DISPLAY_ME_LOOP
	return
TOGGLE_ALARM movlw ALARM_FLAG_TOGGLE
	xorwf myStatus,F;toggle bit alarm flag
	movf alarmMinutes,W
	movwf rightDisplay
	movf alarmHours,W
	movwf leftDisplay
	call CREATE_DISPLAY
TOGGLE_ALARM_LOOP call DISPLAY_ME
	btfsc dipControl,7
	goto TOGGLE_ALARM_LOOP
	return
DO_COMMAND_0 movlw 0x10
	movwf indicator
	movlw 0x3
	movwf leftDisplay
	movlw 0x14
	movwf rightDisplay
	movlw 0x0
	movwf setTimeFSR
	movlw 0xff
	movwf counter
DO_COMMAND_0_LOOP call CREATE_DISPLAY
DO_COMMAND_0_LOOP2 call DISPLAY_ME
	decfsz counter
	goto DO_COMMAND_0_LOOP2
	movf setTimeFSR,W
	bsf STATUS,RP1
	bcf STATUS,RP0
	movwf EEADR
	bsf STATUS,RP0
	bcf EECON1,EEPGD
	bsf EECON1,RD
	bcf STATUS,RP0
	movf EEDATA,W
	bcf STATUS,RP1
	movwf leftDisplay
	bsf STATUS,RP1
	bcf STATUS,RP0
	incf EEADR,F
	bsf STATUS,RP0
	bcf EECON1,EEPGD
	bsf EECON1,RD
	bcf STATUS,RP0
	movf EEDATA,W
	bcf STATUS,RP1
	movwf rightDisplay
	incf setTimeFSR,F
	movlw 0x40
	subwf setTimeFSR,W
	btfss STATUS,Z
	goto DO_COMMAND_0_LOOP
	return
	;
SET_DATE movlw 0x88
	movwf indicator
	movf datePointer,W
	goto SET_TIME
	;
SET_CURRENT_TIME movlw 0x10
	movwf indicator
	movf timePointer,W
	movwf setTimeFSR
	goto SET_TIME
	;
SET_ALARM_TIME movlw 0x18
	movwf indicator
	movf alarmPointer,W
	movwf setTimeFSR
	goto SET_TIME
	;
SET_TIME movwf setTimeFSR
	incf setTimeFSR,F
	movf setTimeFSR,W
	movwf FSR
	movf INDF,W
	movwf leftDisplay
	decf FSR,F
	movf INDF,W
	movwf rightDisplay
	call CREATE_DISPLAY
	movf setTimeFSR,W
	movwf FSR
SET_ALARM_LOOP1	call DISPLAY_ME
	btfsc dipControl,7
	goto SET_ALARM_LOOP1
	movf setTimeFSR,W
	movwf FSR
	movf dipControl,W
	movwf tmp
	bcf tmp,7
	movf tmp,W
	movwf INDF
	movwf leftDisplay
	call CREATE_DISPLAY
	decf setTimeFSR,F
	movf setTimeFSR,W
SET_ALARM_LOOP2 call DISPLAY_ME
	btfss dipControl,7
	goto SET_ALARM_LOOP2
	movf setTimeFSR,W
	movwf FSR
	movf dipControl,W
	movwf tmp
	bcf tmp,7
	movf tmp,W
	movwf INDF
	movwf rightDisplay
	call CREATE_DISPLAY
SET_ALARM_LOOP3 call DISPLAY_ME
	btfsc dipControl,7
	goto SET_ALARM_LOOP3
	movf minutes,W
	movwf rightDisplay
	movf hours,W
	movwf leftDisplay
	call CREATE_DISPLAY
	return
	;
SHOW_DATE movlw 0x88
	movwf indicator
	movf dateDay,W
	movwf rightDisplay
	movf dateMonth,W
	movwf leftDisplay
	call CREATE_DISPLAY
SHOW_DATE_LOOP call DISPLAY_ME
	btfsc dipControl,7
	goto SHOW_DATE_LOOP
	return
	;
	END

	;1.0:	added defines
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
