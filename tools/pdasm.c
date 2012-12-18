/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 20 June 2012
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 * 
 * Prototyping for the PICLANG assembler.
 */

#include "picos/tools/pasm.h"

#include "picos/piclang.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>


picos_size_t FS_BUFFER_SIZE = 128;
int label_counter = 0;

static picos_size_t get_next_word(FILE *hex_file)
{
  picos_size_t retval = -1;

  if(hex_file == NULL || feof(hex_file))
    return retval;

  fread(&retval,sizeof(picos_size_t),1,hex_file);
  return retval;
}

void dump_data(FILE *hex_file, FILE *assembly_file,PCB *pcb)
{
  struct assembly_map* asmb = NULL;
  picos_size_t word,arg;
  int arg_counter;
  int have_string = 0;
  int is_label = 0;
  if(hex_file == NULL || assembly_file == NULL || pcb == NULL)
    return;

  while(!feof(hex_file))
    {
      word = get_next_word(hex_file);
      if(word == 0xdead || word == 0xadde)
	continue;
      if(((word & 0xff) == 0xde) || ((word & 0xff) == 0xad))
	{
	  if(feof(hex_file))
	    break;
	  fseek(hex_file,-1,SEEK_CUR);
	  continue;
	}

      // String section?
      if(ftell(hex_file)>= pcb->string_address*FS_BUFFER_SIZE)
	{
	  char byte_counter;
	  if(!have_string)
	    {
	      fprintf(assembly_file,"\t\"");
	      have_string = 1;
	    }
	  have_string = 1;
	  if(word == 0xdead || word == 0xadde)
	    continue;
	  byte_counter = 0;
	  for(;byte_counter < 2;byte_counter++)
	    {
	      if((word&0xff) != 0xad && (word & 0xff) != 0xde)
		{
		  if((word & 0xff) == 0)
		    {
		      fprintf(assembly_file,"\"\n");
		      have_string = false;
		    }
		  else
		    fprintf(assembly_file,"%c",(char)(word&0xff));
		}
	      word >>= 8;
	    }
	  if(feof(hex_file))
	    break;
	  continue;
	}// End string parser
      if((word & 0xff) == 0xde)
	{
	  word >>= 8;
	  fseek(hex_file,11,SEEK_CUR);
	}
      asmb = opcode2assembly((int)word);
      if(asmb == NULL)
	{
	  fprintf(stderr,"Unknown Opcode: %d\n",(int)word);
	  continue;
	}
      arg_counter = asmb->has_arg;

      // Special argument cases
      switch(word)
	{
	case PICLANG_JMP:case PICLANG_JZ:
	  is_label = true;
	  break;
	default:
	  break;
	}

      // Cases where there is no assembly name
      if(asmb->opcode == PICLANG_NUM_COMMANDS)
	{
	  switch(word)
	    {
	    case PICLANG_LABEL:case PASM_SUBROUTINE:
	      fprintf(assembly_file,"L%03d:",(int)(ftell(hex_file)-pcb->start_address*FS_BUFFER_SIZE-2) >> 1);
	      break;
	    case PICLANG_CALL:case PICLANG_JMP:case PICLANG_JZ:
	      fprintf(assembly_file,"\tcall");
	      is_label = true;
	      arg_counter++;
	      break;
	    case PICLANG_RETURN:
	      fprintf(assembly_file,"\treturn");
	      break;
	    case PICLANG_ARGC:
	      fprintf(assembly_file,"\targc");
	      break;
	    case PICLANG_ARGV:
	      fprintf(assembly_file,"\targv");
	      break;
	    case PICLANG_ERRNO:
	      fprintf(assembly_file,"\terrno");
	      break;
	    case PICLANG_DEREF:
	      fprintf(assembly_file,"\tderef");
	      break;
	    case 0xdead:case 0xadde:
	      continue;
	      break;
	    default:
	      fprintf(assembly_file,"\tUNK(0x%x,0x%x)",(int)asmb->opcode,(int)word);
	      break;
	    }
	}
      else
	fprintf(assembly_file,"\t%s",asmb->keyword);
      if(arg_counter)
	{
	  for(;arg_counter > 0;arg_counter--)
	    {
	      arg = get_next_word(hex_file);
	      if(!is_label)
		fprintf(assembly_file,"\t0x%x",(int)arg);
	      else
		{
		  fprintf(assembly_file,"\tL%03d",(int)arg);
		  is_label = false;
		}
	      if(arg_counter > 0 && asmb->has_arg > 1)
		fprintf(assembly_file,",");
	    }
	}
      fprintf(assembly_file,"\n");
      
    }
  
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


static const char short_options[] = "a:b:hpv";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
	       {"asm", 1,NULL, 'a'},
	       {"block_size",1,NULL,'b'},
	       {"pcb",0,NULL,'p'},
	       {"version",0,NULL,'v'},
               {0, 0, 0, 0}
             };

void print_help()
{
  printf("\n");
  printf("pdasm -- Piclang disassembler.\n");
  printf("Copyright 2011 David Coss, PhD\n");
  printf("-------------------------------\n");
  printf("Decompiles piclang programs into piclang assembly.\n");
  printf("\n");
  printf("Usage: pdasm [options] [binary file]\n\n");
  printf("Options:\n");
  printf("--help, -h :\t\t Displays this dialog.\n");
  printf("--asm,-a <file> :\t Outputs the assembly to the specified file.\n");
  printf("--block_size,- <file> :\t Sets the block size of the binary.\n");
}

void fwrite_pcb(FILE *output,PCB *pcb)
{
	if(output == NULL)
	       output = stdout;
	if(pcb == NULL)
	{
		fprintf(stderr,"fwrite_pcb: NULL pointer for pcb\n");
		return;
	}

	fprintf(output,"Page Size: %d\n",pcb->page_size);
  	fprintf(output,"Bitmap: 0x%x\n",pcb->bitmap);
  	fprintf(output,"Number of Pages: %d\n",pcb->num_pages);
  	fprintf(output,"Initial Program Counter: %d\n",pcb->pc);
  	fprintf(output,"Starting page of data: %d\n",pcb->start_address);
  	fprintf(output,"Starting page of strings: %d\n",pcb->string_address);

	fflush(output);


}

int main(int argc, char **argv) 
{
  FILE *hex_file = stdin, *assembly_file = stdout;
  char opt;
  int opt_index;
  bool dump_pcb = false;
  picos_size_t block_size;
  PCB pcb;

  check_load_rc();
  
  while(true)
    {    
      opt = getopt_long(argc,argv,short_options,long_options,&opt_index);
      if(opt == -1)
	break;
      
      switch(opt)
	{
	case 'a':
	  assembly_file = fopen(optarg,"w");
	  if(assembly_file == NULL)
	    assembly_file = stdout;
	  break;
	case 'b':
	  if(sscanf(optarg,"%hu",&FS_BUFFER_SIZE) != 1)
	    {
	      fprintf(stderr,"Invalid block_size: %s\n",optarg);
	      exit(-1);
	    }
	  break;
	case 'h':
	  print_help();
	  return 0;
	case 'v':
	  printf("%s\n",PACKAGE_STRING);
	  return 0;
	case 'p':
	  dump_pcb = true;
	  break;
	default:
	  fprintf(stderr,"ERROR - Unknown flag %c\n",opt);
	  print_help();
	  return -1;
	}
    }

  if(optind < argc)
    {
      hex_file = fopen(argv[optind++],"r");
      if(hex_file == NULL)
	{
	  fprintf(stderr,"Could not open \"%s\" for reading.\nReason: %s\n",argv[optind-1],strerror(errno));
	  return errno;
	}
    }

  fseek(hex_file,PCB_MAGIC_NUMBER_OFFSET*sizeof(picos_size_t),SEEK_SET);
  fread(&pcb,sizeof(pcb),1,hex_file);
  if(feof(hex_file))
    {
      fprintf(stderr,"Incomplete binary. Error loading PCB.\n");
      exit(-1);
    }

  if(dump_pcb)
  {
	fwrite_pcb(stdout,&pcb);
	return 0;	
  }

  fseek(hex_file,pcb.start_address*FS_BUFFER_SIZE,SEEK_SET);
  if(feof(hex_file))
    {
      fprintf(stderr,"Incomplete binary. No data.\n");
      exit(-1);
    }

  dump_data(hex_file,assembly_file,&pcb);

  return 0;
}
