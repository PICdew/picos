/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 * 
 * Utility functions used within the operating system
 */

#ifndef PICOS_STD
#define PICOS_STD

#include "picos/utils.h"

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
