;Memory map for the pic project.
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
instruction EQU 30h
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
programCounter EQU 5Ah
pclTemp EQU 5Bh
stackTemp EQU 5Ch
accumulator EQU 5Dh
eepromSize EQU 5Eh
dateDay EQU 5Fh
dateMonth EQU 60h
exchange EQU 61h
stackHead EQU 62h
stackTail EQU 6Ah
stackPtr EQU 6Bh
#define LAST_MEMORY_ADDR 0x6b
