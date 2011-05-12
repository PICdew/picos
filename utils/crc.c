#include <stdio.h>
#include "utils.h"


int main(int argc, char **argv)
{
  unsigned char crc = 0;
  char *ch;
  int argidx = 1;
  if(argc == 1)
    return crc;

  for(;argidx < argc;argidx++)
    {
      ch = argv[argidx];
      while(*ch != 0)
	calculate_crc(&crc,*(ch++));

      printf("Word: %s  CRC(hex): %x\n",argv[argidx],crc);
    }
  return (int)crc;
}
