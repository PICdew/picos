#ifndef GLOBAL_H
#define GLOBAL_H 1

#include "pasm.h"

// Globals :-/
extern struct compiled_code *the_code;
extern struct compiled_code *the_code_end;
extern struct compiled_code *the_strings;
extern struct compiled_code *the_strings_end;
extern struct subroutine_map *subroutines;
extern int break_to_label, continue_to_label;
extern idNodeType *variable_list;// Variable table
extern char **string_list;
extern size_t num_strings;
extern picos_size_t FS_BUFFER_SIZE;


#endif

