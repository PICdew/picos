cmake_minimum_required(VERSION 2.6)
project("PICOS Tools" C)
set(PICOS_MAJOR_VERSION 0)
set(PICOS_MINOR_VERSION 6)
set(AUTHOR_EMAIL david@davecoss.com)

# Includes
include_directories( ${PROJECT_SOURCE_DIR}/../../ ${PROJECT_SOURCE_DIR})

# Extra Packages
find_package(BISON)
find_package(FLEX)

# Create config.h
add_definitions(-DHAVE_CONFIG_H)
set(PICOS_BIT_DEFINE int)
configure_file( "${PROJECT_SOURCE_DIR}/config.h.in" "${PROJECT_SOURCE_DIR}/config.h" )

# picosc
BISON_TARGET(picosc_parser picosc_yacc.y ${CMAKE_SOURCE_DIR}/picosc_yacc.c)
FLEX_TARGET(picosc_scanner picosc_lex.l ${CMAKE_SOURCE_DIR}/picosc_lex.c)
ADD_FLEX_BISON_DEPENDENCY(picosc_scanner picosc_parser)
add_executable(picosc compiler.c ${BISON_picosc_parser_OUTPUTS} ${FLEX_picosc_scanner_OUTPUTS} pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c load_config.c)
target_link_libraries(picosc m)

# pasm
BISON_TARGET(pasm_parser pasm_yacc.y ${CMAKE_SOURCE_DIR}/pasm_yacc.c)
FLEX_TARGET(pasm_scanner pasm_lex.l ${CMAKE_SOURCE_DIR}/pasm_lex.c)
ADD_FLEX_BISON_DEPENDENCY(pasm_scanner pasm_parser)
add_executable(pasm compiler.c pasm_lex.c pasm_src.c linker.c ../utils.c ../arg.c ../picfs_error.c picos_base64.c load_config.c ${BISON_pasm_parser_OUTPUTS} ${FLEX_pasm_scanner_OUTPUTS})
target_link_libraries(pasm m)

# pdasm
add_executable(pdasm pdasm.c pasm_src.c load_config.c)

# picdisk
add_executable(picdisk picdisk.c load_config.c)
set_target_properties(picdisk PROPERTIES COMPILE_FLAGS -DPICDISK_MAIN)

# mount.picfs
add_executable(mount.picfs mount_picfs.c pasm_src.c ${PROJECT_SOURCE_DIR}/../utils.c)
set_target_properties(mount.picfs PROPERTIES COMPILE_FLAGS "-DFUSE_USE_VERSION=26 -D_FILE_OFFSET_BITS=64 -I/usr/include/fuse ")
target_link_libraries(mount.picfs pthread m fuse rt dl)

# picos_base64
add_executable(picos_base64 picos_base64.c pasm_src.c)
set_target_properties(picos_base64 PROPERTIES COMPILE_FLAGS -DBASE64_MAIN)

# install
install(TARGETS picosc pasm pdasm picdisk mount.picfs DESTINATION bin)
