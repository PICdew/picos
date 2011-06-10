all: crc

crc: crc.c
	gcc -DNOT_FOR_PIC -o crc crc.c ../utils.c -I../

rsa: rsa.c
	 gcc -o rsa rsa.c -lm