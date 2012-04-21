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
	$(COMP)  -o fs/time time.plc
	$(COMP)  -o fs/start startup_message.plc
	$(COMP)  -o fs/setdate setdate.plc
	$(COMP)  -o fs/settime settime.plc
	$(COMP)  -o fs/test test.plc	
	$(COMP)  -o fs/morse morse.plc
	$(COMP)  -o fs/ed ed.plc
	$(COMP)  -o fs/timestamp timestamp.plc
	$(COMP)  -o fs/calc calc.plc
	$(COMP)  -o fs/base64 base64.plc
	$(COMP)  -o fs/echo echo.plc
	$(COMP)  -o fs/less less.plc
	$(COMP)  -o fs/sleep sleep.plc
	$(COMP)  -o fs/ls ls.plc
	$(COMP)  -o fs/lsmount lsmount.plc

clean:
	-rm -f fs/*

install:
	cp fs/proc/dump $(INTERFACE)/$(IMAGE_NAME)