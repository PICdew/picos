/**
 * PICOS, PIC operating system.
 * Author: David Coss, PhD
 * Date: 1 Sept 2011
 * License: GNU Public License version 3.0 (see http://www.gnu.org)
 *
 * This file contains information on changes related to different version and 
 * revisions. Additionally, this file may be included to reference the version
 * numbers in the program. For version information, see below.
 */


#ifndef PICOS_VERSION_H
#define PICOS_VERSION_H

#define KERNEL_MAJOR_VERSION 1
#define KERNEL_MINOR_VERSION 0
#define KERNEL_REVISION 0
#define KERNEL_ID_TAG 0

#endif

/**
 * 1.0.0 - Switched from assembly to C. The OS grew to such a size as
 *         to make assembly (i.e. paging) unbearable. As of this version,
 *         IO using buttons, USART, LCD and sound word. Commands are 
 *         parsed into CRC unsigned char's.
 *
 * 0.5.1 - Changed shell to run at the end of the process loop. Does not
 *	 check to see if anything owns the lcd anymore. A better process
 *	 of deciding when to use the shell is needed.
 *
 * 0.5.0 - started a shell, which runs when no one owns the LCD.
 *
 * 0.4.1 - Moved page code to its own include file. Made a new translate
 *		 page memory address routine (which needs to be tested).
 * 0.4.0 - Added the beginnings of math routines, including int and float
 *	  math.
 * 	- Created a SWAP function for the stack, which will switch W-reg
 *	  value for the top of the stack.
 *
 * 0.3.0 - Created a version include.
 *	- Stack now can peek, giving the value at the top of the stack
 *	  without popping it.
 *; 0.2.0 - Unfortunately, this file was on updated during 0.2. However, some
 *	  information can be found in the git log.
 *; 0.1.0 - Created process queue system. Processes will have their own portion of memory.
 *		process will run for a certain amount of time and then save their state
 *		and allow another process to run, given the illusion of multitasking.
 *		Process rotation occurs on interrupt. Processes also can allocate
 *		memory in a "free store". This feature needs to be added to piclang.
 *		Also, a garbage collector will be come necessary.
 *
 * 0.0.0 - Reserve memory starting at 70h (see below for length of reserved memory).
 *	- Create a stack that will end at 6fh to be used to store program registers
 *		if a process is suspended. Size is determined by free space, which is equal
 *		to 0x6f - CALL_STACK_LIMIT.
 */
