/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file prototypes the paged memory system used in PICOS.
 */
#ifndef PICOS_PAGE
#define PICOS_PAGE

#include "picos/utils.h"

#ifndef NUM_PAGES
#define NUM_PAGES 4
#endif

#ifndef PAGE_SIZE 
#define PAGE_SIZE 5
#endif

volatile char PAGE_free_bitmap;
volatile picos_size_t PAGE_storage[NUM_PAGES][PAGE_SIZE + 1];// +1 for the program ID, which will be stored in the last byte of the page

/**
 * Initializes the bitmap, where a 1 bit for each index into
 * PAGE_storage[] equals a free array.
 */
void PAGE_init();

/**
 * Requests num_pages pages for the process identified by the integer, pid
 */
char PAGE_request(char num_pages, char pid);

/**
 * Frees all pages associated with pid
 */
void PAGE_free(char pid);

/**
 * Retrieves a byte owned by the pid. The get function will adjust for
 * page boundaries. Thus, the byte index should be the nth byte out of 
 * ALL of total pages listed as one long list from the first page to the
 * last.
 */
picos_size_t PAGE_get(char byte_index, char pid);

/**
 * Sets the byte in page memory to the given value.
 */
void PAGE_set(char byte_index, picos_size_t val, char pid);

#endif // PICOS_PAGE

