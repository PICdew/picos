cmake_minimum_required(VERSION 2.6)
project(PICOS C)
set(PICOS_MAJOR_VERSION 0)
set(PICOS_MINOR_VERSION 6)
set(AUTHOR_EMAIL david@davecoss.com)

add_definitions(-DHAVE_CONFIG_H)
set(PICOS_BIT_DEFINE int)

configure_file( "${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_SOURCE_DIR}/config.h" )
include_directories( ${PROJECT_SOURCE_DIR}/../../ ${PROJECT_SOURCE_DIR})


#AM_YFLAGS = -d 
#BUILT_SOURCES = picosc_yacc.h
#picosc_CFLAGS =  -DNOT_FOR_PIC -I../../
#picosc_SOURCES = compiler.c picosc_lex.l  picosc_yacc.y picos_yacc.h pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c
#picosc_LDADD = -lm
find_package(BISON)
find_package(FLEX)
BISON_TARGET(picosc_parser picosc_yacc.y ${CMAKE_SOURCE_DIR}/picosc_yacc.c)
FLEX_TARGET(picosc_scanner picosc_lex.l ${CMAKE_SOURCE_DIR}/picosc_lex.c)
ADD_FLEX_BISON_DEPENDENCY(picosc_scanner picosc_parser)
add_executable(picosc compiler.c ${BISON_picosc_parser_OUTPUTS} ${FLEX_picosc_scanner_OUTPUTS} pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c load_config.c)
target_link_libraries(picosc m)


#pasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../../
#pasm_SOURCES = compiler.c pasm_lex.l  pasm_yacc.y pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c
#pasm_LDADD = -lm

#pdasm_CFLAGS = -Wunused-result -DNOT_FOR_PIC -I../../
#pdasm_SOURCES = pdasm.c pasm_src.c

#picdisk_CFLAGS = -Wunused-result -DNOT_FOR_PIC -DPICDISK_MAIN -I../../
#picdisk_SOURCES = picdisk.c


#mount_picfs_SOURCES = mount_picfs.c pasm_src.c ../utils.c
#mount_picfs_CFLAGS = -I../../ -DNOT_FOR_PIC -DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse 
#mount_picfs_LDADD = -lm  -lfuse -lrt -ldl 
#mount_picfs_LDFLAGS = -pthread

#picos_base64_SOURCES = picos_base64.c pasm_src.c
#picos_base64_CFLAGS = -DBASE64_MAIN -I../../


install(TARGETS picosc DESTINATION bin)
