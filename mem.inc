#define END_OF_MEMORY low endOfMemory
#define STACK_LENGTH 0x8
;Memory map for the pic project.
	cblock 0x20
output
counter
tmp
myStatus
saveW
minutes
hours
currSeg
valueToLatch
;total minutes since start of timer. why not? might come in handy later.
;Middle of a 3byte counter of total minutes
;end of total minutes segment. increment LOW, if carryover increment middle, if carryover increment high. if carryover... whoa!
totalMinutesLOW: 0,totalMinutesMIDDLE,totalMinutesHIGH
setTimeTmp
resetTMR0;TMR0's initial value
hexToOctal;memory spot for bit rotation
instruction
lsdMinute:0,msdMinute;7-seg packets for digits
lsdHour:0,msdHour
binaryMinute:0,binaryHours;binary packets
firstDisplay:0,lastDisplay;7-seg data
alarmMinutes:0,alarmHours;Alarm Clock! cf myStatus
leftDisplay:0,rightDisplay;Display place holders to make create display more generic. to show time, load minutes/hours into it.
indicator;which decimal to light (tells a story)
programCounter
pclTemp
stackTemp
accumulator
exchange
eepromSize
dateDay:0,dateMonth
stackHead:STACK_LENGTH
stackPtr
endOfMemory
	endc
	
INIT_MEMORY_MAC macro endOfMemory
	movlw END_OF_MEMORY
	movwf endOfMemory;So I can keep track of when my "name space" ends
	endm