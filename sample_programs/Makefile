BLOCK_SIZE=255

all:
	pasm -b $(BLOCK_SIZE) -o fs/time time.plc
	pasm -b $(BLOCK_SIZE) -o fs/start startup_message.plc
	pasm -b $(BLOCK_SIZE) -o fs/setdate setdate.plc
	pasm -b $(BLOCK_SIZE) -o fs/settime settime.plc
	pasm -b $(BLOCK_SIZE) -o fs/test test.plc	
	pasm -b $(BLOCK_SIZE) -o fs/morse morse.plc
	pasm -b $(BLOCK_SIZE) -o fs/ed ed.plc
	pasm -b $(BLOCK_SIZE) -o fs/timestamp timestamp.plc
	pasm -b $(BLOCK_SIZE) -o fs/calc calc.plc
	pasm -b $(BLOCK_SIZE) -o fs/base64 base64.plc
	pasm -b $(BLOCK_SIZE) -o fs/echo echo.plc
	pasm -b $(BLOCK_SIZE) -o fs/less less.plc
	pasm -b $(BLOCK_SIZE) -o fs/sleep sleep.plc
