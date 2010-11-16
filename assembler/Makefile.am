bin_PROGRAMS = assembler simulator

assembler_SOURCES = Build.cpp Command.cpp CommandWords.cpp assembler.cpp Parser.cpp Help.cpp \
	DString.cpp StringTokenizer.cpp good_exceptions.cpp
assembler_CPPFLAGS = ${LIBDNSTD_CPPFLAGS}
assembler_LDFLAGS = ${LIBDNSTD_LDFLAGS} ${LIBREADLINE}
assembler_LDADD = ${LIBDNSTD_LDADD}

simulator_SOURCES = Build.cpp Command.cpp CommandWords.cpp simulator.cpp Parser.cpp Help.cpp \
	DString.cpp StringTokenizer.cpp good_exceptions.cpp
simulator_CPPFLAGS = ${LIBDNSTD_CPPFLAGS}
simulator_LDFLAGS = ${LIBDNSTD_LDFLAGS} ${LIBREADLINE}
simulator_LDADD = ${LIBDNSTD_LDADD}