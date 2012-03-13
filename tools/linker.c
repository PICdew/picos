#include "pasm.h"


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
      if(code->type == typePCB)
	continue;
      if(code->type == typeLabel)
	{
	  if(code->label == label)
	    return code_counter;
	}
      code_counter++;
    }
  return -1;
}

void resolve_labels(struct compiled_code* code)
{
  const struct compiled_code* code_head = code;
  if(code == NULL)
    return;
  
  for(;code != NULL;code = code->next)
    {
      switch(code->val)
	{
	case PICLANG_JMP:case PICLANG_JZ:case PICLANG_CALL:// change labels to addresses
	  if(code->next != NULL)
	    {
	      int label_addr, subroutine_label;
	      // Next next word should contain the target address.
	      // 1.) If the type of the next word is a subroutine, the label
	      //     of the subroutine must be found. That label is used
	      //     to find the target *address*.
	      // 2.) Otherwise the next byte is the label itself, which is
	      //     used to find the target *address*.
	      if(code->next->val == PASM_SUBROUTINE)
		{
		  const struct subroutine_map *subroutine = lookup_subroutine(code->next->label);
		  if(subroutine == NULL || subroutine->label == -1)
		    {
		      if(subroutine == NULL)
			fprintf(stderr,"Undefined reference: #%d\n",code->next->label);
		      else
			fprintf(stderr,"Undefined reference: %s\n",subroutine->name);
		      exit(-1);
		    }
		  label_addr = lookup_label(code_head, subroutine->label);
		}
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
	      int label_addr = lookup_label(code_head, code->next->next->val);
	      if(label_addr < 0)
		{
		  fprintf(stderr,"Could not resolve label %d\n",code->next->next->val);
		  return;
		}
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

