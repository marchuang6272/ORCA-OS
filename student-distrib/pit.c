/* Contains init and handler functions for the global timer (IRQ 0) */
#include "pit.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "idt.h"
#include "scheduling.h"

void pit_init(int frequency) {
    cli();
    enable_irq(0);
    extern void pit_linkage(void);
    SET_IDT_ENTRY(idt[0x20], pit_linkage);
    set_idt(0x20);
    /* calculates reload value */
    uint16_t reload_value = 1193182/frequency;
    /* Program PIT channel */
    outb(0x34, 0x43);   //channel 0, lobyte/hibyte, rate generator
    outb(reload_value&0x00FF, 0x40); // low byte
	outb((reload_value&0xFF00)>>8, 0x40);	// high byte
}

void pit_handler()
{
    scheduler();
    send_eoi(0);
}


