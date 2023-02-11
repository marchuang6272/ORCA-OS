#ifndef _H_PAGING_
#define _H_PAGING_

#include "types.h"

// Page directory and table structures are defined in x86_desc.h

/**
 * @brief A function that initializes the page directory and table.
 * No inputs or outputs, but does initializes the paging
 * 
*/
void page_init(void);

/**
 * @brief Initializes the paging during system calls, specifically during execute and halt.
 * @input A pointer to the current PCB (of type pcb_t, defined in types.h)
 * 
*/
void system_call_paging(pcb_t* pcb_pointer);

void terminal_switch_paging(int terminal_num);
void scheduler_paging(int terminal_num);

/**
 * @brief Initializes the paging during the vidmap system call.
 * No inputs, but updates paging and flushes the TLB.
 * 
*/
void system_call_vidmap(void);

#endif
/*** end of file ***/
