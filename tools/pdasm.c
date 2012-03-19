#include "pasm.h"

#include "../piclang.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <stdbool.h>

picos_size_t FS_BUFFER_SIZE = 128;
int label_counter = 0;

static picos_size_t get_next_word(FILE *hex_file)
{
  picos_size_t retval = -1;
  if(hex_file != NULL)
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
	  if(feof(hex_file))
	    {
	      fprintf(assembly_file,"\"\n");
	      break;
	    }
	  if(word == 0xdead || word == 0xadde)
	    continue;
	  byte_counter = 0;
	  for(;byte_counter < 2;byte_counter++)
	    {
	      if((word&0xff) != 0xad && (word & 0xff) != 0xde)
		{
		  if(word & 0xff == 0)
		    {
		      fprintf(assembly_file,"\"\n");
		      have_string = false;
		    }
		  else
		    fprintf(assembly_file,"%c",(char)(word));
		}
	      word >>= 8;
	    }
	  continue;
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
	      fprintf(assembly_file,"L%03d:",(ftell(hex_file)-pcb->start_address*FS_BUFFER_SIZE-2) >> 1);
	      break;
	    case PICLANG_CALL:case PICLANG_JMP:case PICLANG_JZ:
	      fprintf(assembly_file,"\tcall");
	      is_label = true;
	      arg_counter++;
	      break;
	    case PICLANG_RETURN:
	      fprintf(assembly_file,"\treturn");
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


static const char short_options[] = "a:b:h";
enum OPTION_INDICES{OUTPUT_HEX};
static struct option long_options[] =
             {
	       {"help",0,NULL,'h'},
	       {"asm", 1,NULL, 'a'},
	       {"block_size",1,NULL,'b'},
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

int main(int argc, char **argv) 
{
  FILE *hex_file = stdin, *assembly_file = stdout;
  char opt;
  int opt_index;
  picos_size_t block_size;
  PCB pcb;
  
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

  fseek(hex_file,pcb.start_address*FS_BUFFER_SIZE,SEEK_SET);
  if(feof(hex_file))
    {
      fprintf(stderr,"Incomplete binary. No data.\n");
      exit(-1);
    }

  dump_data(hex_file,assembly_file,&pcb);

  return 0;
}
