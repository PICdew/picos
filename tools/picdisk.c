/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 20 June 2012
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 * 
 * Utility program for examining PICFS images
 */

#include "fs.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>

int FS_BUFFER_SIZE = 128;

int scan_disk(FILE *image_file)
{
  FS_Unit *super_block = NULL;
  long fpos;
  int image_counter = 1;
  if(image_file == NULL)
    return ENOENT;
  
  super_block = (FS_Unit*)malloc(FS_SuperBlock_length*sizeof(FS_Unit));
  if(super_block == NULL)
    {
      fprintf(stderr,"Could not allocate super block buffer.\n");
      if(errno != 0)
	{
	  fprintf(stderr,"Reason: %s\n",strerror(errno));
	  exit(errno);
	}
      exit(1);
    }

  while(!feof(image_file))
    {
      // Get super block
      fpos = ftell(image_file);
      fread(super_block,sizeof(FS_Unit),FS_SuperBlock_length,image_file);
      if(feof(image_file))
	break;
      fseek(image_file,fpos,SEEK_SET);

      if(image_counter > 1)
	printf("\n\n");
      
      if(super_block[FS_SuperBlock_magic_number] != 0 || super_block[FS_SuperBlock_magic_number2] != 6 || super_block[FS_SuperBlock_magic_number3] != 0x29 || super_block[FS_SuperBlock_magic_number4] != 0x82)
	{
	  if(fpos == 0)
	    {
	      fprintf(stderr,"Corrupted disk image at %ld. Invalid magic number.\n",fpos);
	      free(super_block);
	      return 1;
	    }
	  else
	    {
	      long the_eof;
	      fseek(image_file,0,SEEK_END);
	      the_eof = ftell(image_file);
	      printf("Free space beginning at %ld: %ld\n",fpos, the_eof-fpos);
	      free(super_block);
	      return 0;
	    }
	}
      
      // print info
      printf("Image #%d\n",image_counter);
      printf("PICFS version %d\n",(int)super_block[FS_SuperBlock_revision_num]);
      printf("Block size: %d\n",(int)super_block[FS_SuperBlock_block_size]);
      printf("Number of Blocks: %d\n",(int)super_block[FS_SuperBlock_num_blocks]);
      printf("Number of Free Blocks: %d\n",(int)super_block[FS_SuperBlock_num_free_blocks]);
      printf("Root block: %ld\n",(long)super_block[FS_SuperBlock_root_block]*super_block[FS_SuperBlock_block_size] + fpos);
      printf("Starting address: %ld\n",fpos);
      printf("Contains raw file: %s\n",((super_block[FS_SuperBlock_raw_file])?"yes":"no"));
      printf("Total Size: %ld\n",(long)super_block[FS_SuperBlock_block_size]*super_block[FS_SuperBlock_num_blocks]);

      image_counter++;

      // skip to next image
      fseek(image_file,super_block[FS_SuperBlock_block_size]*super_block[FS_SuperBlock_num_blocks],SEEK_CUR);
    }
  
  free(super_block);
  return 0;
}

void load_rc(char *keyword, char *arg)
{
  size_t len,idx;
  int have_arg, iarg;

  if(keyword == NULL || strlen(keyword) == 0)
    return;
  
  have_arg = (arg != NULL) & strlen(arg);

  len = strlen(keyword);idx = 0;
  for(;idx < len;idx++)
    {
      if(keyword[idx] == ' ')
	{
	  keyword[idx] = '\0';
	  break;
	}
      keyword[idx] = tolower(keyword[idx]);
    }
  
  if(have_arg)
    {
      len = strlen(arg);idx = 0;
      for(;idx < len;idx++)
	{
	  if(arg[idx] == ' ')
	    {
	      arg[idx] = '\0';
	      break;
	    }
	  arg[idx] = tolower(arg[idx]);
	}
    }

  if(strcmp(keyword,"block_size") == 0)
    {
      if(have_arg)
	{
	  if(sscanf(arg,"%d",&iarg) != 1)
	    fprintf(stderr,"Invalid block size \"%s\"\n",arg);
	  else
	    FS_BUFFER_SIZE = (picos_size_t)iarg;
	}
      else
	fprintf(stderr,"No argument for BLOCK_SIZE\n");
    }
}


void check_load_rc()
{
  char *home_dir = NULL;
  const size_t buffer_size = 2048;
  char rc_path[FILENAME_MAX], buf[buffer_size];
  FILE *rc_file = NULL;
  char *keyword, *arg, *saveptr;

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
      *arg = 0;arg++;
      while(*arg == ' ')
	arg++;
      if(*arg == 0)
	continue;
      
      load_rc(keyword,arg);
    }
  
  fclose(rc_file);
}


static const char short_options[] = "h";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
               {0, 0, 0, 0}
             };

void print_help()
{
  printf("\n");
  printf("picdisk -- PICOS disk utility.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Utility function for managing PICOS disk images.\n");
  printf("\n");
  printf("Usage: pdasm [options] [image file]\n\n");
  printf("Options:\n");
  printf("--help, -h :\t\t Displays this dialog.\n");
}

int main(int argc, char **argv)
{
  char opt;
  int opt_index;
  FILE *image_file = NULL;

  
  check_load_rc();

    while(true)
    {    
      opt = getopt_long(argc,argv,short_options,long_options,&opt_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case 'h':
	  print_help();
	  return 0;
	default:
	  fprintf(stderr,"ERROR - Unknown flag %c\n",opt);
	  print_help();
	  return -1;
	}
    }

    if(optind < argc)
      {
	image_file = fopen(argv[optind++],"r");
	if(image_file == NULL)
	  {
	    fprintf(stderr,"Could not open \"%s\" for reading.\nReason: %s\n",argv[optind-1],strerror(errno));
	    return errno;
	  }
      }

    return scan_disk(image_file);

}
