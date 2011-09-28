#ifndef PICOS_STD
#define PICOS_STD

#include "utils.h"

/**
 * Handler for mounting a file system from the command line.
 */
signed char picos_mount(picos_size_t addr);

/**
 * Handler for changing active file system from the command line.
 */
signed char picos_chdir(picos_size_t addr);

/**
 * millisecond delay
 */
unsigned int millisleep(unsigned int ms);


#endif
