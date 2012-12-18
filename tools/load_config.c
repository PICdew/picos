#include "load_config.h"

#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void check_load_rc()
{
  char *home_dir = NULL;
  const size_t buffer_size = 2048;
  char rc_path[FILENAME_MAX], buf[buffer_size];
  FILE *rc_file = NULL;
  char *keyword, *arg, *saveptr;

  extern void load_rc(char *keyword, char *arg);

  home_dir = getenv("HOME");
  
  if(home_dir == NULL)
    {
      fprintf(stderr,"No home directory.\n");
      return;
    }
  
  sprintf(rc_path,"%s/.picfsrc",home_dir);

  if(access(rc_path,R_OK) != 0)
    {
      // Cannot read rc file. Moving on...
      return;
    }
  
  rc_file = fopen(rc_path,"r");
  
  while(!feof(rc_file))
    {
      if(fgets(buf,buffer_size,rc_file) == NULL)
	break;
      arg = strrchr(buf,'\n');
      if(arg == NULL)
	continue;
      if(arg)
	*arg = '\0';
      arg = strrchr(buf,'\r');
      if(arg)
	*arg = '\0';
      arg = strchr(buf,'#');
      if(arg)
	*arg = '\0';
      keyword = buf;
      if(*keyword == 0)
	continue;
      while(*keyword == ' ')
	keyword++;
      if(strlen(keyword) == 0)
	continue;
      
      arg = strchr(keyword,' ');
      if(arg == NULL)
	continue;
      *arg = 0;arg++;
      while(*arg == ' ')
	arg++;
      if(*arg == 0)
	continue;
      
      load_rc(keyword,arg);
    }
  
  fclose(rc_file);
}
