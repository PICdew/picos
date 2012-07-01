bin_PROGRAMS = picosc pdasm picdisk mount.picfs picos_base64

AM_YFLAGS = -d 
BUILT_SOURCES = picosc_yacc.h
picosc_CFLAGS =  -DNOT_FOR_PIC -I../../
picosc_SOURCES = compiler.c picosc_lex.l  picosc_yacc.y picos_yacc.h pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c
picosc_LDADD = -lm


#pasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../
#pasm_SOURCES = compiler.c pasm_lex.l  pasm_yacc.y pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c
#pasm_LDADD = -lm

pdasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../../
pdasm_SOURCES = pdasm.c pasm_src.c

picdisk_CFLAGS = -Wunused-result -DNOT_FOR_PIC -DPICDISK_MAIN -I../../
picdisk_SOURCES = picdisk.c


mount_picfs_SOURCES = mount_picfs.c pasm_src.c ../utils.c
mount_picfs_CFLAGS = -I../../ -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
mount_picfs_LDADD = -lm  -lfuse -lrt -ldl 
mount_picfs_LDFLAGS = -pthread

picos_base64_SOURCES = picos_base64.c pasm_src.c
picos_base64_CFLAGS = -DBASE64_MAIN -I../../
