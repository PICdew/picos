#include "pasm.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

const struct subroutine_map* lookup_subroutine(int index)
{
  extern struct subroutine_map *subroutines;
  const struct subroutine_map *retval = subroutines;
  while(retval != NULL)
    {
      if(retval->index == index)
	break;
      retval = retval->next;
    }
  
  return retval;
}

int lookup_label(const struct compiled_code* code, picos_size_t label)
{
  int code_counter = 0;
  if(code == NULL)
    return -1;
  for(;code != NULL;code = code->next)
    {
      if(code->type == typePCB || code->type == typeStr || code->type == typePad)
	continue;
      if(code->type == typeLabel && (code->val == PICLANG_LABEL))
	{
	  if(code->label == label)
	    return code_counter;
	}
      code_counter++;
    }
  return -1;
}

static int get_subroutine_addr(const struct compiled_code *code_head, const struct compiled_code *code)
{
  const struct subroutine_map *subroutine = lookup_subroutine(code->label);
  if(subroutine == NULL || subroutine->label == -1)
    {
      if(subroutine == NULL)
	fprintf(stderr,"Undefined reference: #%d\n",code->label);
      else
	fprintf(stderr,"Undefined reference: %s\n",subroutine->name);
      exit(-1);
    }
  return lookup_label(code_head, subroutine->label);
  
}

void resolve_labels(struct compiled_code* code)
{
  const struct compiled_code* code_head = code;
  int label_addr, subroutine_label;
  if(code == NULL)
    return;
  
  for(;code != NULL;code = code->next)
    {
      switch(code->val)
	{
	case PICLANG_JMP:case PICLANG_JZ:case PICLANG_CALL:// change labels to addresses
	  if(code->next != NULL)
	    {
	      // Next next word should contain the target address.
	      // 1.) If the type of the next word is a subroutine, the label
	      //     of the subroutine must be found. That label is used
	      //     to find the target *address*.
	      // 2.) Otherwise the next byte is the label itself, which is
	      //     used to find the target *address*.
	      if(code->next->val == PASM_SUBROUTINE)
		label_addr = get_subroutine_addr(code_head, code->next);
	      else
		label_addr = lookup_label(code_head, code->next->label);
	      if(label_addr < 0)
		{
		  fprintf(stderr,"Could not resolve label %d\n",code->next->label);
		  return;
		}
	      code->next->val = (picos_size_t)label_addr;
	      code = code->next;
	      continue;
	    }
	  break;
	case PICLANG_SIGNAL:
	  if(code->next != NULL && code->next->next != NULL)
	    {
	      int label_addr;
	      if(code->next->next->val == PASM_SUBROUTINE)
		label_addr = get_subroutine_addr(code_head, code->next->next);
	      else
		label_addr = lookup_label(code_head, code->next->next->label);
	      code->next->next->val = (picos_size_t)label_addr;
	      code = code->next->next;
	      continue;
	    }
	  break;
	default:
	  break;
	}
      if((opcode2assembly(code->val))->has_arg)
	code = code->next;
    }
}


void create_lst_file(FILE *lst_file, const struct compiled_code *the_code, const struct compiled_code *the_strings)
{
  const struct compiled_code *curr_code = NULL;
  if(lst_file != NULL)
    {
      struct assembly_map* curr;
      const struct compiled_code *first_string = NULL;
      int code_counter = 0;
      curr_code = the_code;
      for(;curr_code != NULL;curr_code = curr_code->next)
	{
	  if(curr_code->type == typePCB)
	    continue;
	  if(curr_code->type == typeStr || curr_code->type == typePad)
	    {
	      continue;
	    }
	  curr = opcode2assembly(curr_code->val);
	  fprintf(lst_file,"(%d)\t",code_counter++);
	  switch(curr->opcode)
	    {
	    case PICLANG_NUM_COMMANDS:
	      if(curr_code->type == typeLabel)
		{
		  fprintf(lst_file,"L%03hu",curr_code->label);
		  break;
		}
	      else if(curr_code->val == PICLANG_RETURN)
		{
		  fprintf(lst_file,"return");
		  break;
		}
	      else if(curr_code->val == PICLANG_CALL)
		{
		  curr_code = curr_code->next;
		  while(curr_code->next != NULL && (curr_code->type == typeStr || curr_code->type == typePad))
		    curr_code = curr_code->next;
		  fprintf(lst_file,"call %hu",curr_code->val);
		  code_counter++;
		  break;
		}
	    default:
	      fprintf(lst_file,"%s (0x%x) ",curr->keyword,curr_code->val);
	      break;
	    }
	  if(curr->has_arg)
	    {
	      curr_code = curr_code->next;
	      code_counter++;
	      fprintf(lst_file," %d",curr_code->val);
	      if(curr->has_arg > 1)
		{
		  int arg_counter = 1;
		  for(;arg_counter < curr->has_arg;arg_counter++)
		    {
		      code_counter++;
		      curr_code = curr_code->next;
		      fprintf(lst_file,", %d",curr_code->val);
		    }
		}
	    }
	  fprintf(lst_file,"\n");
	}
      // print strings
      first_string = the_strings;
      if(first_string != NULL)
	fprintf(lst_file,"Strings:\n\"");
      for(;first_string != NULL;first_string = first_string->next)
	{
	  if(first_string->val == 0)
	    {
	      fprintf(lst_file,"\"\n");
	      if(first_string->next != NULL)
		fprintf(lst_file,"\"");
	    }
	  else if(first_string->val == '"')
	    fprintf(lst_file,"\"%c",first_string->val);
	  else
	    fprintf(lst_file,"%c",first_string->val);
	}
    }
}

static struct compiled_code* increment_word(const struct compiled_code *word, int *counter)
{
  if(word == NULL)
    return NULL;
  
  if(counter != NULL)
    *counter += 1;
  return word->next;
}

void create_lnk_file(FILE *lnk_file, const struct compiled_code *the_code)
{
  const struct compiled_code *curr_code = the_code;
  int word_counter = 0,arg_counter;
  struct assembly_map *asmb = NULL;
  if(lnk_file == NULL || curr_code == NULL)
    return;
  
  for(;curr_code != NULL;curr_code = increment_word(curr_code,NULL))
    {
      if(curr_code->type != typeCode && curr_code->type != typeLabel && curr_code->type != typeSubroutine)
	continue;
      word_counter++;
      asmb = opcode2assembly(curr_code->val);
      if(asmb == NULL)
	continue;
      arg_counter = 0;
      switch(curr_code->val)
	{
	case PICLANG_LABEL:
	  fprintf(lnk_file,"Label %d @ %d\n",curr_code->label,word_counter-1);
	  break;
	case PICLANG_JMP: case PICLANG_JZ: case PICLANG_CALL:
	  if(curr_code->next != NULL)
	    fprintf(lnk_file,"%s to %d @ %d\n",asmb->keyword,curr_code->next->val,word_counter);
	  break;
	default:
	  break;
	}
      for(;arg_counter<asmb->has_arg;arg_counter++)
	curr_code = increment_word(curr_code,&word_counter);
    }
}

static picos_size_t lib_get_next_word(FILE *hex_file)
{
  picos_size_t retval = -1;

  if(hex_file == NULL || feof(hex_file))
    return retval;

  fread(&retval,sizeof(picos_size_t),1,hex_file);
  return retval;
}

void reason_exit(char format, ...)
{
  va_list args;
  va_start(args,format);
  vfprintf(stderr,format,args);
  va_end(args);
  
  exit(1);
}

static void insert_relmap_entry(struct relocation_map **map,int addr,int offset, int relocation_type)
{
  struct relocation_map *new_relocate = NULL;
  if(map == NULL)
    {
      fprintf(stderr,"insert_relmap_entry: NULL pointer for map\n");
      exit(1);
    }
  
  new_relocate = (struct relocation_map *)malloc(sizeof(struct relocation_map));
  if(new_relocate == NULL)
    {
      fprintf(stderr,"insert_relmap_entry: could not allocate memory for relocation map\n");
      if(errno)
	{
	  fprintf(stderr,"Reason: %s\n",strerror(errno));
	  exit(errno);
	}
      exit(errno);
    }
  
  new_relocate->relocation.addr = addr;
  new_relocate->relocation.offset = offset;
  new_relocate->relocation.type = relocation_type;
  new_relocate->next = NULL;

  if(*map == NULL)
    *map = new_relocate;
  else
    {
      new_relocate->next = *map;
      *map = new_relocate;
    }
}

static void free_relmap(struct relocation_map *map)
{
  free(map);
}

static void piclib_load_strings(FILE *libfile, struct compiled_code **strings_ptr, size_t num_strings)
{
  char ch;
  struct compiled_code *string_end = NULL;

  if(libfile == NULL)
    reason_error("piclib_load_strings: Null pointer for library file\n");

  if(strings_ptr == NULL)
    return;// assume this means to skip
  
  while(num_strings)
    {
      fread(&ch,sizeof(char),1,libfile);
      if(feof(libfile))
	reason_error("error: Broken piclib file in STRINGS section\n");
      
      insert_compiled_code(typeStr, strings_ptr, &string_end , ch, index_counter);
    }
  
}

struct piclib_object* piclib_load(FILE *libfile)
{
  picos_size_t word;
  const size_t bufsiz = 1024;
  const char block_name_format[] = "%1024s(%d):";
  char buffer[bufsiz], *name_pointer;
  size_t section_size;
  struct piclib_object *retval = NULL;

  if(libfile == NULL)
    {
      fprintf(stderr,"piclib_load: Null pointer for libfile\n");
      return NULL;
    }


  // Check magic numbers
  fread(buffer,strlen(PICLANG_LIB_MAGIC_NUMBERS),sizeof(char),libfile);
  if(strncmp(buffer,PICLANG_LIB_MAGIC_NUMBERS,strlen(PICLANG_LIB_MAGIC_NUMBERS)) != 0)
    {
      fprintf(stderr,"piclib_load: File is not a PICLIB file\n");
      return NULL;
    }

  retval = (struct piclib_object*)malloc(sizeof(struct piclib_object));
  if(retval == NULL)
    reason_exit("piclib_load: Could not allocate memory for piclib object.\n");
  
  while(!feof(libfile)
    {
      if(fscanf(libfile,block_name_format,buffer,&section_size) == 0)
	reason_exit("error: Invalid piclib block\n");

      printf("Loading section: \"%s\" Size: %d\n",buffer,section_size);
      if(strncmp(buffer,"STRINGS",strlen("STRINGS")) == 0)
	piclib_load_strings(libfile,&retval->strings);
      else if(strncmp(buffer,"SUBROUTINES",strlen("SUBROUTINES")) == 0)
	 fseek(libfile,section_size*sizeof(char),SEEK_CUR);
      else if(strncmp(buffer,"STRINGS",strlen("STRINGS")) == 0)
	 fseek(libfile,section_size*sizeof(char),SEEK_CUR);
      else if(strncmp(buffer,"STRINGS",strlen("STRINGS")) == 0)
	 fseek(libfile,section_size*sizeof(char),SEEK_CUR);
      else 
	{
	  fprintf(stderr,"Unknown section\n");
	  exit(1);
	}
    }

#if 0 // OLD!!!  
  // initialize struct
  retval = (struct piclib_object*)malloc(sizeof(struct piclib_object));
  retval->offset = 0;
  memset(retval->filename,0,FILENAME_MAX);
  retval->next = NULL;
  retval->code = retval->strings = NULL;
  
  // Load subroutine map
  fread(buffer,strlen("FUNCTS:"),sizeof(char),libfile);
  if(strncmp(buffer,"FUNCTS:",sizeof("FUNCTS")) != 0 || feof(libfile))
    {
      fprintf(stderr,"Corrupt PICLIB file. Missing function header\n");
      exit(1);
    }

  // setup subroutines (FUNCTS)
  while(!feof(libfile))
    {
      fscanf(libfile,"%c",buffer);
      if(*buffer == '-')
	break;// no FUNCTS?!?
      
      if(*buffer == '<')
	{
	  open_tag = true;
	  curr_subroutine = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
	  memset(curr_subroutine->name,0,FILENAME_MAX);
	  name_pointer = curr_subroutine->name;
	  curr_subroutine->index = index_counter++;
	  curr_subroutine->next = NULL;
	  curr_subroutine->label = 0;// this will be filled in after the subroutine's name
	}
      else if(*buffer == '>')
	{
	  if(retval->subroutines == NULL)
	    retval->subroutines = curr_subroutine;
	  else
	    {
	      curr_subroutine->next = retval->subroutines;
	      retval->subroutines = curr_subroutine;
	    }
	  curr_subroutine = NULL;
	  open_tag = false;
	}
      else if(open_tag)
	{
	  // Filling name
	  *name_pointer = *buffer;
	  name_pointer++;
	}
      else
	{
	  // this is part of the subroutine's address
	  if(*buffer == ';')
	    break;// end of FUNCT section
	  if(*buffer > '9' || *buffer < '0')
	    {
	      fprintf(stderr,"Invalid subroutine address digit: \"%c\"",*buffer);
	      free(retval);
	      return NULL;
	    }
	  retval->subroutines->label *= 10;
	  retval->subroutines->label += (*buffer - 0x30);
	}
    }// end of FUNCT loading while
  
  fread(buffer,strlen("STRINGS:"),sizeof(char),libfile);
  if(strncmp(buffer,"STRINGS:",strlen("STRINGS:")) != 0)
    {
      fprintf(stderr,"Invalid piclib header");
      free(retval);
      return NULL;
    }
  while(!feof(libfile))
    {
      fscanf(libfile,"%c",buffer);
      if(*buffer == '-')
	break;// no strings
      if(*buffer == ';')
	break;// end of string section
      if(*buffer > '9' || *buffer < '0')
	{
	  fprintf(stderr,"Invalid string address digit: \"%c\"\n",*buffer);
	  free(retval);
	  return NULL;
	}	  
      if(string_loc == -1)
	string_loc = 0;
      string_loc *= 10;
      string_loc += (*buffer - 0x30);
    }
  
  // Code section
  fread(buffer,strlen("CODE:"),sizeof(char),libfile);
  if(strncmp(buffer,"CODE:",strlen("CODE:")) != 0)
    {
      fprintf(stderr, "Corrupt PICLIB file. Missing CODE tag\n");
      free(retval);
      return NULL;
    }

  index_counter = 0;
  string_end = code_end = NULL;
  while(!feof(libfile))
    {
      code_word = lib_get_next_word(libfile);
      if(feof(libfile))
	break;

      if(index_counter >= string_loc)
	insert_compiled_code(typeStr, &retval->strings, &string_end , code_word, index_counter);
      else
	insert_compiled_code(typeCode, &retval->code, &code_end , code_word, index_counter);
      
      index_counter++;
    }

  // relocation map section
  fread(buffer,strlen("RELMAP:"),sizeof(char),libfile);
  if(strncmp(buffer,"RELMAP:",strlen("RELMAP:")) != 0)
    {
      fprintf(stderr, "Corrupt PICLIB file. Missing RELMAP tag\n");
      free(retval);
      return NULL;
    }
  num_relmaps = 0;
  fread(buffer,sizeof(char),1,libfile);
  if(*buffer != '-')
    {
      // have rel maps. load them.
      while(*buffer != '=' && !feof(libfile))
	{
	  if(*buffer < '0' || *buffer > '9')
	    {
	      fprintf(stderr,"piclib_load: Invalid RELMAP size digit '%c'\n",*buffer);
	      exit(1);
	    }
	  
	  num_relmaps *= 10;
	  num_relmaps += (*buffer - '0');
	}
    }
  while(num_relmaps > 0)
    {
      fread(&relocation,sizeof(relocation_t),1,libfile);
      if(feof(libfile))
	{
	  fprintf(stderr,"piclib_load: Incomplete RELMAP section. Expected %d more entries.\n",num_relmaps);
	  exit(1);
	}
      if(relmap == NULL)
	{
	  relmap = (struct relocation_map *)malloc(sizeof(struct relocation_map));
	  relmap->next = NULL;
	}
      else
	{
	  struct relocation_map *tmpmap = (struct relocation_map *)malloc(sizeof(struct relocation_map));
	  tmpmap->next = relmap;
	  relmap = tmpmap;
	}
      relmap->relocation = relocation;
      num_relmaps--;
    }

#endif// OLD!!!  

  return retval;
    
}

static void create_lib_header(FILE *binary_file, const struct compiled_code *curr_code, const struct compiled_code *the_strings)
{
  const struct compiled_code *head = curr_code;
  int word_counter = 0;
  bool have_type = false;
  
  fprintf(stderr,"create_lib_header: DEPRECATED!\n");
  exit(1);

  if(binary_file == NULL || curr_code == NULL)
    return;
  
}

void piclib_write_subroutines(FILE *binary_file, const struct compiled_code *curr_code)
{
  size_t word_counter = 0;
  const struct compiled_code *code_head = curr_code;

  if(binary_file == NULL || curr_code == NULL)
    return;

  while(curr_code != NULL)
    {
      if(curr_code->type == typeLabel && (curr_code->val == PICLANG_LABEL))
	{
	  const struct subroutine_map *subroutine = lookup_subroutine(curr_code->label);
	  if(subroutine != NULL)
	    word_counter++;
	}
      curr_code = curr_code->next;
    }

  fprintf(binary_file,"SUBROUTINES(%lu):",word_counter);
  if(word_counter == 0)
    return;
	      
  curr_code = code_head;
  word_counter = 0;
  while(curr_code != NULL)
    {
      if(curr_code->type == typeLabel && (curr_code->val == PICLANG_LABEL))
	{
	  const struct subroutine_map *subroutine = lookup_subroutine(curr_code->label);
	  if(subroutine != NULL)
	    fprintf(binary_file,"<%s>%d",subroutine->name,word_counter);
	}
      curr_code = curr_code->next;
      word_counter++;
    }

}

void piclib_write_strings(FILE *binary_file, const struct compiled_code *curr_code)
{
  size_t word_counter = 0;
  const struct compiled_code *code_head = curr_code;  

  if(binary_file == NULL || curr_code == NULL)
    return;
  
  while(curr_code != NULL)
    {
      if(curr_code->type == typeStr)
	word_counter++;
      curr_code = curr_code->next;
    }

  fprintf(binary_file,"STRINGS(%lu):",word_counter);
  
  curr_code = code_head;
  while(curr_code)
    {
      if(curr_code->type == typeStr)
	fprintf(binary_file,"%c",(char)curr_code->val);
      curr_code = curr_code->next;
    }

}

void write_piclib_obj(FILE *binary_file,const struct compiled_code *libcode,const struct compiled_code *libstrings)
{
  const struct compiled_code *curr_code;
  struct relocation_map *relmap = NULL, *tmpmap;// for relocation table
  int word_counter = 0;
  
  if(binary_file == NULL)
    return;
  
  //create_lib_header(binary_file, libcode,libstrings);
  fprintf(binary_file,"%s",PICLANG_LIB_MAGIC_NUMBERS);

  // Subroutine map
  piclib_write_subroutines(binary_file,libcode);

  // Strings
  piclib_write_strings(binary_file,libstrings);
  
  // Write Code section
  fprintf(binary_file,"CODE(%lu):",CountCode(libcode));
  curr_code = libcode;
  while(curr_code != NULL)
    {
      if(curr_code->type != typeStr && curr_code->type != typePad)
	{
	  write_val_for_pic(binary_file,curr_code->val);
	  switch(curr_code->val)
	    {
	    case PICLANG_PUSH: case PICLANG_POP:
	      if(curr_code->next == NULL)
		{
		  fprintf(stderr,"write_piclib_obj: NULL pointer for next word of push/pop\n");
		  exit(1);
		}
	      curr_code = curr_code->next;
	      insert_relmap_entry(&relmap,word_counter,curr_code->val,REL_VARIABLE);
	      break;
	    default:
	      break;
	    }
	}

      curr_code = curr_code->next;
    }

  tmpmap = relmap;
  word_counter = 0;
  while(tmpmap != NULL)
    {
      word_counter++;
      tmpmap = tmpmap->next;
    }
         
  fprintf(binary_file,"RELMAP(%d):",word_counter);
  tmpmap = relmap;
  while(tmpmap != NULL)
    {
      fwrite(&tmpmap->relocation,sizeof(relocation_t),1,binary_file);
      tmpmap = tmpmap->next;
    }
  tmpmap = (struct relocation_map *)malloc(sizeof(struct relocation_map));
  memset(&tmpmap->relocation,0,sizeof(relocation_t));
  fwrite(&tmpmap->relocation,sizeof(relocation_t),1,binary_file);
  free(tmpmap);
  
  // Free structs
  tmpmap = relmap;
  while(tmpmap != NULL)
    {
      relmap = tmpmap;
      tmpmap = tmpmap->next;
      free(relmap);
    }

}




