;picos kernel 0.0.0
;author: David Coss
;Date: 10/2/10
;License: GNU Public License version 3.0 (see http://www.gnu.org)
;
#define KERNEL_MAJOR_VERSION 0
#define KERNEL_MINOR_VERSION 0
#define KERNEL_REVISION 0

START_INTERRUPT macro saveW
	movwf saveW;save w value
	endm
	
FINISH_INTERRUPT macro saveW
	movf saveW,W
	bcf INTCON,2
	endm

MAIN_PROCESS_MAC macro CREATE_DISPLAY,DISPLAY_ME,controlPort,PROGRAM_MODE,MAIN_LOOP
MAIN_PROCESS call CREATE_DISPLAY
	movlw 0xff
	movwf counter
	call DISPLAY_ME
	decfsz counter,F
	goto $-2
	clrwdt;WDT checks for run away code. If I get this far, I'm not running away
PROGRAM_MAIN_FORK	btfsc controlPort,PROGRAM_MODE_BIT
	goto PROGRAM_MODE
	goto MAIN_LOOP ; repeat....
	endm
	
	
	
