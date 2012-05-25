#include "pasm.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

static const char piclib_subroutine_format[] = "%s %lu %lu ";
static   const char block_name_format[] = "%s %d:";

const struct subroutine_map* lookup_subroutine(int index)
{
  extern struct subroutine_map *global_subroutines;
  extern struct subroutine_map *subroutines;
  const struct subroutine_map *retval = global_subroutines;
  while(retval != NULL)
    {
      if(retval->address == index)
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
#if 0// Old Deprecated.
const struct subroutine_map *subroutine = lookup_subroutine(code->label);
  if(subroutine == NULL || subroutine->address == -1)
    {
      if(subroutine == NULL)
	fprintf(stderr,"Undefined reference: #%d\n",code->label);
      else
	fprintf(stderr,"Undefined reference: %s\n",subroutine->name);
      exit(-1);
    }
  return lookup_label(code_head, subroutine->address);
#endif

 const struct subroutine_map *subroutine = (struct subroutine_map*)code->target; 
 if(subroutine == NULL || subroutine->address == -1)
 {
	if(subroutine == NULL)
		fprintf(stderr,"Undefined reference: #%d\n",code->label);
	else
	        fprintf(stderr,"Undefined reference: %s\n",subroutine->name);
	        exit(1);
 }

 return subroutine->address;

}

void resolve_labels(struct compiled_code* code, int address_offset, int variable_offset)
{
  const struct compiled_code* code_head = code;
  int label_addr, subroutine_label, arg_counter;
  if(code == NULL)
    return;
  
  for(;code != NULL;code = code->next)
    {
	    if(code->type == typeId)
	    {
		    fprintf(stderr,"HAVE LABEL!!!\n");
		    code->val += variable_offset;
		    continue;
	    }

	    // Type is typeCode
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
	      code->next->val = (picos_size_t)label_addr + address_offset;
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
	      code->next->next->val = (picos_size_t)label_addr + address_offset;
	      code = code->next->next;
	      continue;
	    }
	  break;
	default:
	  break;
	}
      
      arg_counter = 0;
      for(;arg_counter < (opcode2assembly(code->val))->has_arg;arg_counter++)
      {
	code = code->next;
	if(code == NULL)
		break;
	if(code->type == typeId)
	    {
		    fprintf(stderr,"HAVE LABEL!!!\n");
		    code->val += variable_offset;
		    continue;
	    }
      }
      if(code == NULL)
	      break;// code could become null in arg_counter loop
    }// end code for loop
}


void create_lst_file(FILE *lst_file, const struct compiled_code *the_code)
{
  const struct compiled_code *curr_code = NULL;
  const struct compiled_code *the_strings;


  if(lst_file != NULL)
    {
      struct assembly_map* curr;
      int code_counter = 0;
      bool have_strings = false;
      curr_code = the_code;
      for(;curr_code != NULL;curr_code = curr_code->next)
	{
	  if(curr_code->type == typePCB)
	    continue;
	  if(curr_code->type == typePad)
	    continue;
	  if(curr_code->type == typeStr) 
	    {
		    if(!have_strings)
		    {
			    fprintf(lst_file,"Strings:\n");
			    have_strings = true;
		    }
	      fprintf(lst_file,"%c",curr_code->val);
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
      fprintf(lst_file,"\n");
      fflush(lst_file);
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
  new_relocate->relocation.val = offset;
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

void piclib_free(struct piclib_object *library)
{
	struct compiled_code *code;
	struct subroutine_map *subroutine;
        struct relocation_map *relmap;

	if(library == NULL)
		return;

	while(library->code != NULL)
	{
		code = library->code->next;
		free(library->code);
		library->code = code;
	}

	while(library->strings != NULL)
	{
		code = library->strings->next;
		free(library->strings);
		library->strings = code;
	}

	while(library->subroutines != NULL)
	{
		subroutine = library->subroutines->next;
		free(library->subroutines);
		library->subroutines = subroutine;
	}

	while(library->relmap != NULL)
	{
		relmap = library->relmap->next;
		free(library->relmap);
		library->relmap = relmap;
	}


}

#if 0//FIX!
static void piclib_load_strings(FILE *libfile, struct subroutine_map *subroutine, size_t num_strings)
{
  char ch;
  struct compiled_code *string_end = NULL;

  if(libfile == NULL)
    reason_exit("piclib_load_strings: Null pointer for library file\n");

  if(strings_ptr == NULL)
    return;// assume this means to skip
  
  while(num_strings)
    {
      fread(&ch,sizeof(char),1,libfile);
      if(feof(libfile))
	reason_exit("error: Broken piclib file in STRINGS section\n");
      
      insert_compiled_code(typeStr, strings_ptr, &string_end , ch, 0x42);
      num_strings--;
    }
  
}

static void piclib_load_relmap(FILE *libfile, struct relocation_map **relmap_ptr, size_t num_relmaps)
{

  relocation_t relocation;

  if(libfile == NULL)
    reason_exit("piclib_load_subroutines: Null pointer for library file\n");

  if(relmap_ptr == NULL)
    return;// assume this means to skip

  while(num_relmaps > 0)
    {
      fread(&relocation,sizeof(relocation_t),1,libfile);
      if(feof(libfile))
	{
	  fprintf(stderr,"piclib_load: Incomplete RELMAP section. Expected %lu more entries.\n",num_relmaps);
	  exit(1);
	}
      if(relmap_ptr == NULL)
	{
	  *relmap_ptr = (struct relocation_map *)malloc(sizeof(struct relocation_map));
	  (*relmap_ptr)->next = NULL;
	}
      else
	{
	  struct relocation_map *tmpmap = (struct relocation_map *)malloc(sizeof(struct relocation_map));
	  tmpmap->next = *relmap_ptr;
	  *relmap_ptr = tmpmap;
	}
      (*relmap_ptr)->relocation = relocation;
      num_relmaps--;
    }

}

static void piclib_load_code(FILE *libfile, struct compiled_code **code_ptr, size_t num_words)
{
  struct compiled_code *code_end, *code_word;
  int index_counter = 0;

  if(libfile == NULL)
    reason_exit("piclib_load_code: Null pointer for library file\n");

  if(code_ptr == NULL)
    return;// assume this means to skip

  code_end = *code_ptr;
  if(code_end != NULL)
    {
      while(code_end->next != NULL)
	code_end = code_end->next;
    }

  while(!feof(libfile) && num_words != 0)
  {
    code_word = (struct compiled_code*)malloc(sizeof(struct compiled_code));
    if(code_word == NULL)
      reason_exit("piclib_load_code: could not allocate memory for struct compiled_code.\n");
    
    if(fread(code_word,sizeof(struct compiled_code),1,libfile) == 0)
      reason_exit("error: Could not load CODE word\n");
    code_word->next = NULL;
    
    if(code_end == NULL)
      {
	*code_ptr = code_word;
	code_end = *code_ptr;
      }
    else
      {
	code_end->next = code_word;
	code_end = code_word;
      }
    
      num_words--;
  }


}
static void piclib_load_subroutines(FILE *libfile, struct subroutine_map **subroutines_ptr, size_t num_subs)
{
  char sub_name[FILENAME_MAX];
  struct subroutine_map *subs_end = NULL;
  //struct subroutine_map tmp;
  size_t sub_label;
  size_t sub_index;
  
  if(libfile == NULL)
    reason_exit("piclib_load_subroutines: Null pointer for library file\n");

  if(subroutines_ptr == NULL)
    return;// assume this means to skip
  
  while(num_subs)
    {
      fscanf(libfile,piclib_subroutine_format,sub_name,&sub_label, &sub_index);
      if(feof(libfile))
	reason_exit("error: Broken piclib file in SUBROUTINES section\n");
     
      subs_end = *subroutines_ptr;
      *subroutines_ptr = (struct subroutine_map*)malloc(sizeof(struct subroutine_map));
      //(*subroutines_ptr) = tmp;
      strncpy((*subroutines_ptr)->name,sub_name,FILENAME_MAX);
      (*subroutines_ptr)->label = sub_label;
      (*subroutines_ptr)->index = sub_index;
      (*subroutines_ptr)->next = subs_end;

      num_subs--;

    }
}

struct piclib_object* piclib_load(FILE *libfile)
{
  picos_size_t word;
  const size_t bufsiz = 1024;
  char buffer[bufsiz], *name_pointer;
  int section_size;
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
  
  while(!feof(libfile))
    {
      if(fscanf(libfile,block_name_format,buffer,&section_size) == 0)
	reason_exit("error: Invalid piclib block\n");

      if(feof(libfile))
	      break;

      printf("Loading section: \"%s\" Size: %d\n",buffer,section_size);
      if(strncmp(buffer,"STRINGS",strlen("STRINGS")) == 0)
	piclib_load_strings(libfile,&retval->strings,section_size);
      else if(strncmp(buffer,"SUBROUTINES",strlen("SUBROUTINES")) == 0)
	piclib_load_subroutines(libfile,&retval->subroutines, section_size);
      else if(strncmp(buffer,"CODE",strlen("CODE")) == 0)
	 piclib_load_code(libfile,&retval->code, section_size);
      else if(strncmp(buffer,"RELMAP",strlen("RELMAP")) == 0)
	 piclib_load_relmap(libfile, &retval->relmap, section_size);
      else 
	{
	  fprintf(stderr,"Unknown section\n");
	  exit(1);
	}
    }


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
  int word_counter = 0;
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

  fprintf(binary_file,block_name_format,"SUBROUTINES",word_counter);
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
	    fprintf(binary_file,piclib_subroutine_format,subroutine->name,subroutine->label, subroutine->index );
	}
      curr_code = curr_code->next;
      word_counter++;
    }

}

void piclib_write_strings(FILE *binary_file, const struct compiled_code *curr_code)
{
  int word_counter = 0;
  const struct compiled_code *code_head = curr_code;  

  if(binary_file == NULL || curr_code == NULL)
    return;
  
  while(curr_code != NULL)
    {
      if(curr_code->type == typeStr)
	word_counter++;
      curr_code = curr_code->next;
    }

  fprintf(binary_file,block_name_format,"STRINGS",word_counter);
  
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
  word_counter = 0;
  curr_code = libcode;
  while(curr_code != NULL)
  {
	  if(curr_code->type != typeStr && curr_code->type != typePad)
	  {
		  word_counter++;
	  }
	  curr_code = curr_code->next;
  }
  fprintf(binary_file,block_name_format,"CODE",word_counter);
  curr_code = libcode;
  word_counter = 0;
  while(curr_code != NULL)
    {
      if(curr_code->type != typeStr && curr_code->type != typePad)
	{
	  //write_val_for_pic(binary_file,curr_code->val);
	  fwrite(curr_code,sizeof(struct compiled_code),1,binary_file);
	  switch(curr_code->val)
	    {
		    case PICLANG_LABEL:
		      insert_relmap_entry(&relmap,word_counter,0,REL_LABEL);
		      break;
	    case PICLANG_PUSH: case PICLANG_POP:
	      if(curr_code->next == NULL)
		{
		  fprintf(stderr,"write_piclib_obj: NULL pointer for next word of push/pop\n");
		  exit(1);
		}
	      curr_code = curr_code->next; word_counter++;
	      //write_val_for_pic(binary_file,curr_code->val);
	      fwrite(curr_code,sizeof(struct compiled_code),1,binary_file);
	      insert_relmap_entry(&relmap,word_counter,curr_code->val,REL_VARIABLE);
	      break;
	    case PICLANG_JMP: case PICLANG_JZ: case PICLANG_CALL:
	      if(curr_code->next == NULL)
		{
		  fprintf(stderr,"write_piclib_obj: NULL pointer for next word of jmp/jz/call\n");
		  exit(1);
		}
	      curr_code = curr_code->next; word_counter++;
	      //write_val_for_pic(binary_file,curr_code->val);
	      fwrite(curr_code,sizeof(struct compiled_code),1,binary_file);
	      insert_relmap_entry(&relmap,word_counter,0,REL_LABEL);
	      break;
	    default:
	      if(curr_code->relocation_type != -1)
		      insert_relmap_entry(&relmap,word_counter,0,curr_code->relocation_type);
	      break;
	    }

	}

      curr_code = curr_code->next;
      word_counter++;
    }
 
  tmpmap = relmap;
  word_counter = 0;
  while(tmpmap != NULL)
    {
      word_counter++;
      tmpmap = tmpmap->next;
    }
         
  fprintf(binary_file,block_name_format,"RELMAP",word_counter);
  tmpmap = relmap;
  while(tmpmap != NULL)
    {
      fwrite(&tmpmap->relocation,sizeof(relocation_t),1,binary_file);
      tmpmap = tmpmap->next;
    }
   fprintf(binary_file,"%c",0);

   // Free structs
  tmpmap = relmap;
  while(tmpmap != NULL)
    {
      relmap = tmpmap;
      tmpmap = tmpmap->next;
      free(relmap);
    }

}

struct compiled_code* piclib_get_word(struct compiled_code *code, size_t nth_word)
{
  
	if(code == NULL)
		return NULL;

	while(code != NULL)
	{
		if(nth_word == 0)
			return code;
		nth_word--;
		code = code->next;
	}

	return NULL;
}

int piclib_link(struct piclib_object *library, struct subroutine_map *subroutines)
{
  idNodeType* resolve_variable(const char *name);
  size_t library_offset;
	picos_size_t label, label_offset, string_offset;
	struct relocation_map *relmap = NULL;
	struct compiled_code *curr_word = NULL;
	struct subroutine_map *subs = NULL;
	extern picos_size_t label_counter;
	struct compiled_code **the_code_ptr, **the_strings_ptr, **code_end, **strings_end;

        if(subroutines == NULL)
		return -1;

	the_code_ptr = &subroutines->code;
	the_strings_ptr = &subroutines->strings;
	code_end = &subroutines->code_end;
	strings_end = &subroutines->strings_end;

	if(library == NULL || the_code_ptr == NULL || the_strings_ptr == NULL)
	{
		fprintf(stderr,"piclib_link: NULL pointer\n");
		return -1;
	}

	library_offset = CountCode(*the_code_ptr);
	string_offset = CountCode(*the_strings_ptr);
	label_offset = label_counter;

	

	relmap = library->relmap;
	if(library->code != NULL){
	while(relmap != NULL)
	{
		curr_word = piclib_get_word(library->code,relmap->relocation.addr);
		if(curr_word == NULL)
			reason_exit("error: Invalid relocation address %d\n",relmap->relocation.addr);

		switch(relmap->relocation.type)
		{
			case REL_VARIABLE:
				{
					char name[FILENAME_MAX+4];
					idNodeType *var;
					snprintf(name,FILENAME_MAX+3,"%s_%03d",library->filename,relmap->relocation.val);
					var = resolve_variable(name);
					if(var == NULL)
						reason_exit("error: Could not allocate variable for library file \"%s\"\"");
					curr_word->val = var->i;
					printf("Setting variable at %d in %d\n",relmap->relocation.addr,var->i);
					curr_word->type = typeId;
					break;
				}
			case REL_LABEL:
				{
					curr_word->val += library_offset;
					printf("Changing label address from %d to %d\n",curr_word->val-library_offset,curr_word->val);
					if(curr_word->val == PICLANG_LABEL)
						label_counter++;
					break;
#if 0//needed?
					curr_word->label += label_offset;
					printf("Setting label(%d) at %d from %d to %d\n",curr_word->val,relmap->relocation.addr, curr_word->label - label_offset, curr_word->label);
					curr_word->type = typeLabel;
					if(curr_word->val == PICLANG_LABEL)
						label_counter++;
					curr_word->val = PICLANG_LABEL;
					break;
#endif
				}
			case REL_STRING:
				curr_word->val += string_offset;
				printf("Setting string pointer (%d) to %d\n",curr_word->val - string_offset,curr_word->val);
				break;
			default:
				printf("Skipping relocation type %d\n",relmap->relocation.type);
				break;
		}
		
		relmap = relmap->next;
	}
	
	curr_word = library->code;
	label = library_offset;
	while(curr_word != NULL)
	{
		insert_compiled_code(curr_word->type, the_code_ptr, code_end, curr_word->val, curr_word->label);
		curr_word = curr_word->next;
	}
	}

	subs = library->subroutines;
	while(subs != NULL)
	{
		printf("Inserting subroutine %s with index %lu and label %lu\n",subs->name,subs->index, subs->label);
		insert_subroutine(subs->name,subs->label + label_offset);
		label_counter++;
		subs = subs->next;
	}

	curr_word = library->strings;
	while(curr_word != NULL)
	{
		insert_compiled_code(typeStr,the_strings_ptr,strings_end,curr_word->val,0);
		printf("Inserted String (%d) %c\n",curr_word->val,curr_word->val);
		curr_word = curr_word->next;
	}
	
	return 0;
}

#endif //FIX

