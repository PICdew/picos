bin_PROGRAMS = pasm fs #psim

AM_YFLAGS = -d
pasm_CFLAGS = ${PICOS_CFLAGS} -DNOT_FOR_PIC
pasm_SOURCES = pasm_lex.l  pasm_yacc.y pasm_src.c utils.c
pasm_LDADD = -lm

fs_SOURCES = fs.c pasm_src.c
fs_CFLAGS = ${PICOS_CFLAGS} -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
fs_LDADD = -lm  -lfuse -lrt -ldl 
fs_LDFLAGS = -pthread

#psim_SOURCES = psim_lex.l psim_yacc.y psim_src.c utils.c
#psim_CFLAGS = ${PICOS_CFLAGS} -DNOT_FOR_PIC
#psim_LDADD = -lm