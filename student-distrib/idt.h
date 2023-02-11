/* idt.h - Defines used in interactions with idt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _IDT_H
#define _IDT_H

#include "lib.h"

/* set idt helper funciton */
void set_idt(uint8_t i);

/* Initialize both PICs */
void idt_init();

#endif /* _IDT_H */
