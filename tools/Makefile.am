bin_PROGRAMS = picosc pasm fs

AM_YFLAGS = -d 
picosc_CFLAGS = -g -O0 -DNOT_FOR_PIC -I../
picosc_SOURCES = picosc_lex.l  picosc_yacc.y pasm_src.c ../utils.c ../arg.c ../picfs_error.c
picosc_LDADD = -lm
picosc_LDFLAGS = -g -O0

pasm_CFLAGS = -g -O0 -DNOT_FOR_PIC -I../
pasm_SOURCES = picosc_lex.l  pasm_yacc.y pasm_src.c ../utils.c ../arg.c ../picfs_error.c
pasm_LDADD = -lm
pasm_LDFLAGS = -g -O0

fs_SOURCES = fs.c pasm_src.c ../utils.c
fs_CFLAGS = -I../ -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
fs_LDADD = -lm  -lfuse -lrt -ldl 
fs_LDFLAGS = -pthread
