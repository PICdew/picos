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
	
FINISH_INTERRUPT macro saveW,INTCON
	movf saveW,W
	bcf INTCON,2
	endm
	
	
	
