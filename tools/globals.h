#ifndef GLOBAL_H
#define GLOBAL_H 1

#include "pasm.h"

// Globals :-/
extern struct subroutine_map *global_subroutines, *g_curr_subroutine, *string_handler;
extern struct subroutine_map *global_subroutines_GLOBALS;// this will contain global variables and strings.
extern int break_to_label, continue_to_label;
extern idNodeType *variable_list;// Variable table
extern char **string_list;
extern size_t num_strings;
extern picos_size_t FS_BUFFER_SIZE;


#endif

