#define DEFAULT_DECIMAL_VALUE 0x10;code for decimal on 7-seg
#define LEFT_DECIMAL_VALUE 0x10
#define RIGHT_DECIMAL_VALUE 0x8
#define MSN_RIGHT_DECIMAL_VALUE 0x18
#define DISPLAY_MEMORY_WIDTH 0x8

INIT_DISPLAY_MAC macro firstDisplay,lastDisplay,indicator
	movlw DEFAULT_DECIMAL_VALUE;by default light 2nd decimal (from left)
	movwf indicator
	movlw low lsdMinute
	movwf firstDisplay
	movlw low binaryHours
	movwf lastDisplay
	movlw 0x2
	subwf lastDisplay
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
	endm

CREATE_DISPLAY_MAC macro myStatus,minutes,hours,MAKE_PACKET,binaryMinute,binaryHours,controlPort,rightDisplay,SEG_VALUES,output,LOAD_PACKET,indicator,hexToOctal 
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
	btfss myStatus,HEX_OCTAL_BIT
	movlw b'111000'
	andwf rightDisplay,W
	bcf STATUS,C
	movwf hexToOctal
	rrf hexToOctal,F
	rrf hexToOctal,F
	rrf hexToOctal,F
	btfsc myStatus,HEX_OCTAL_BIT
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
	btfss myStatus,HEX_OCTAL_BIT
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
	btfsc myStatus,HEX_OCTAL_BIT
	rrf hexToOctal,F
	movf hexToOctal,W
	call SEG_VALUES
	bcf myStatus, LSN_MSG_7SEG_BIT;left 7-seg
	movwf output
	movlw MSN_RIGHT_DECIMAL_VALUE
	call LOAD_PACKET		
	movf indicator,W;light decimal
	movwf INDF
	return
	endm
	
;Loads a 7-seg packet
LOAD_PACKET_MAC macro firstDisplay,currSeg,MAKE_PACKET,output,indicator 
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
	endm
	
;Creates a 7-seg packet
MAKE_PACKET_MAC macro tmp,myStatus 
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
	endm

SEG_VALUES_MAC macro pclTemp
SEG_VALUES movwf pclTemp
	movlw HIGH SEG_VALUES_TABLE
	movwf PCLATH
	movf pclTemp,W
	addwf PCL,F
SEG_VALUES_TABLE retlw b'01111110';0
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
	endm
	
DISPLAY_ME_MAC macro firstDisplay,outport,binaryHours,lastDisplay 
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
	endm