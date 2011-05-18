bin_PROGRAMS = pasm #psim

AM_YFLAGS = -d
pasm_CFLAGS = ${PICOS_CFLAGS} -DNOT_FOR_PIC
pasm_SOURCES = pasm_lex.l  pasm_yacc.y pasm_src.c utils.c
pasm_LDADD = -lm

#psim_SOURCES = psim_lex.l psim_yacc.y psim_src.c utils.c
#psim_CFLAGS = ${PICOS_CFLAGS} -DNOT_FOR_PIC
#psim_LDADD = -lm