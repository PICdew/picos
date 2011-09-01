/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file provides code for handling errors within the PICOS.
 */
#include "picfs_error.h"

signed char error_return(char the_error)
{
  error_code = the_error;
  return -1;
}


