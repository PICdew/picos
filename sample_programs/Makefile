ifndef BLOCK_SIZE
BLOCK_SIZE=255
endif
ifndef COMP
COMP=../tools/picosc
endif
ifndef INTERFACE
INTERFACE=../../interface
endif
ifndef IMAGE_NAME
IMAGE_NAME=sdcard
endif

all:
	$(COMP) -b $(BLOCK_SIZE) -o fs/time time.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/start startup_message.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/setdate setdate.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/settime settime.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/test test.plc	
	$(COMP) -b $(BLOCK_SIZE) -o fs/morse morse.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/ed ed.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/timestamp timestamp.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/calc calc.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/base64 base64.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/echo echo.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/less less.plc
	$(COMP) -b $(BLOCK_SIZE) -o fs/sleep sleep.plc

clean:
	rm -f fs/*

install:
	cp fs/proc/dump $(INTERFACE)/$(IMAGE_NAME)