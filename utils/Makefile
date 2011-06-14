all: crc rsa

crc: crc.c
	gcc -DNOT_FOR_PIC -o crc crc.c ../utils.c -I../

rsa: rsa.c ../rsa.c
	 gcc -o rsa rsa.c ../rsa.c -lm

clean: 
	rm -f crc rsa