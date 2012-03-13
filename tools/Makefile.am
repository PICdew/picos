bin_PROGRAMS = picosc pasm pdasm fs

AM_YFLAGS = -d 
picosc_CFLAGS =  -DNOT_FOR_PIC -I../
picosc_SOURCES = compiler.c picosc_lex.l  picosc_yacc.y pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c
picosc_LDADD = -lm
picosc_LDFLAGS = 

pasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../
pasm_SOURCES = compiler.c pasm_lex.l  pasm_yacc.y pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c
pasm_LDADD = -lm
pasm_LDFLAGS = 

pdasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../
pdasm_SOURCES = pdasm.c pasm_src.c


fs_SOURCES = fs.c pasm_src.c ../utils.c
fs_CFLAGS = -I../ -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
fs_LDADD = -lm  -lfuse -lrt -ldl 
fs_LDFLAGS = -pthread
