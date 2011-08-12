#include "picfs_error.h"

signed char error_return(char the_error)
{
  error_code = the_error;
  return -1;
}
