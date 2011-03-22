bin_PROGRAMS = pasm simulator

pasm_SOURCES =  pasm_yacc.y pasm_lex.l pasm_src.c 
pasm_LDADD = -lm
AM_YFLAGS = -d

simulator_SOURCES = Build.cpp Command.cpp CommandWords.cpp simulator.cpp Parser.cpp Help.cpp \
	DString.cpp StringTokenizer.cpp good_exceptions.cpp
simulator_CPPFLAGS = ${LIBDNSTD_CPPFLAGS}
simulator_LDFLAGS = ${LIBDNSTD_LDFLAGS} ${LIBREADLINE}
simulator_LDADD = ${LIBDNSTD_LDADD}