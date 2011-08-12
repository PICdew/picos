#include "page.h"
#include "picfs_error.h"

void PAGE_init()
{
  char counter = 0;
  PAGE_free_bitmap = 1 << NUM_PAGES;
  PAGE_free_bitmap--;
  for(;counter < NUM_PAGES;counter++)
    PAGE_storage[counter][PAGE_SIZE] = 0;// 0 = free 
}

char PAGE_request(char num_pages, char pid)
{
  char counter = 0, mask;
  if(PAGE_free_bitmap == 0)
    {
      error_code = PAGE_INSUFFICENT_MEMORY;
      return -1;
    }
  for(;counter < NUM_PAGES;counter++)
    {
      if(num_pages == 0)
	break;
      mask = (1 << counter);
      if((PAGE_free_bitmap & mask) != 0)
	{
	  PAGE_free_bitmap &= ~mask;
	  PAGE_storage[counter][PAGE_SIZE] = pid;
	  num_pages--;
	}
    }
  if(num_pages != 0)
    {
      PAGE_free(pid);
      error_code = PAGE_INSUFFICENT_MEMORY;
      return -1;
    }
  return 0;
  
}

void PAGE_free(char pid)
{
  char counter = 0, mask;
  for(;counter < NUM_PAGES;counter++)
    {
      mask = (1 << counter);
      if(PAGE_storage[counter][PAGE_SIZE] == pid)
	{
	  PAGE_free_bitmap |= mask;
	  PAGE_storage[counter][PAGE_SIZE] = 0;
	}
    }
}

char PAGE_get(char byte_index, char pid)
{
  char page_index = byte_index / PAGE_SIZE;
  byte_index = byte_index % PAGE_SIZE;
  if(page_index >= NUM_PAGES)
    {
      error_code = PAGE_NO_SUCH_ADDRESS;
      return -1;
    }
  
  if(PAGE_storage[page_index][PAGE_SIZE] != pid)
    {
      error_code = PAGE_PERMISSION_DENIED;
      return -1;
    }
  return PAGE_storage[page_index][byte_index];  
}

void PAGE_set(char byte_index, char val, char pid)
{
  char page_index = byte_index / PAGE_SIZE;
  byte_index = byte_index % PAGE_SIZE;
  if(page_index >= NUM_PAGES)
    {
      error_code = PAGE_NO_SUCH_ADDRESS;
      return;
    }
  
  if(PAGE_storage[page_index][PAGE_SIZE] != pid)
    {
      error_code = PAGE_PERMISSION_DENIED;
      return;
    }
  PAGE_storage[page_index][byte_index] = val;  
}
