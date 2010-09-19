;Requires eeprom1 and eeprom0 to be a real memory locations
;which will be the EEADDR to read from or to write to.

;BEGIN CUSTOMIZATION
#define EEPROM_SIZE 0x40
#define NUM_INSTRUCTIONS 0x1

;Runs the command corresponding to the value of "instruction".
;The return value is place at the top of the stack (i.e. stack)
RUN_COMMAND movlw NUM_INSTRUCTIONS
	subwf instruction,W
	btfsc
	addwf PCL,F
;TODO CHECK TO SEE IF THE REQUESTED INSTRUCTION IS BEYOND THE NUMBER
;OF INSTRUCTIONS. THEN FILL IN TABLE.

;END CUSTOMIZATION

;Reads data from EEPROM.
;Input: eeprom0 = eeprom data address
;Output: eeprom0 = data stored at eeprom address. Bank 0
;is used after return
READ_EEPROM bsf STATUS,RP1;bank 2
	bcf STATUS,RP0
	movf eeprom0,W
	movwf EEADR
	bsf STATUS,RP0;bank 3
	bcf EECON1,EEPGD
	bsf EECON1,RD
	bcf STATUS,RP0;bank 2
	movf EEDATA,W
	movwf eeprom0
	bcf STATUS,RP1;bank 0
	return
	
;Writes data to EEPROM
;Input: eeprom0 = eeprom addres
;		eeprom1 = data to be written
;Output: W = data that was written
WRITE_EEPROM bsf STATUS,RP1;bank 3
	bsf STATUS,RP0
	btfsc EECON1,WR;wait for the last write to finish
	goto $-1
	bcf STATUS,RP0;bank 2
	movf eeprom0,W
	movwf EEADR
	movf eeprom1,W
	movwf EEDATA
	bsf STATUS,RP0;bank 3
	bcf EECON1,EEPGD;use eeprom, not program flash
	bsf EECON1,WREN;enable write
	BCF INTCON,GIE;temporarily disable interrupts
	movlw 0x55
	movwf EECON2
	movlw 0xaa
	movwf EECON2
	bsf EECON1,WR;start writting
	bsf INTCON,GIE;re-enable interrupts
	bcf EECON1,WREN;disable write.
	bcf STATUS,RP0;bank 0
	bcf STATUS,RP1
	movf eeprom1,W
	return
	