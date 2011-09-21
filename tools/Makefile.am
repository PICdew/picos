bin_PROGRAMS = pasm fs

AM_YFLAGS = -d
pasm_CFLAGS = -DNOT_FOR_PIC -I../
pasm_SOURCES = pasm_lex.l  pasm_yacc.y pasm_src.c ../utils.c ../arg.c
pasm_LDADD = -lm

fs_SOURCES = fs.c pasm_src.c ../utils.c
fs_CFLAGS = -I../ -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
fs_LDADD = -lm  -lfuse -lrt -ldl 
fs_LDFLAGS = -pthread
