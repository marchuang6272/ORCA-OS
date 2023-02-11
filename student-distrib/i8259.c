/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    master_mask = 0xFB;
    slave_mask = 0xFF;

    // starts the initialization sequence (in cascade mode)
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    // ICW2: Master PIC vector offset
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(ICW3_MASTER, MASTER_8259_DATA);
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(ICW3_SLAVE, SLAVE_8259_DATA);

    // ICW4: tell that ISA=x86, normal/auto EOI
    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    // restore saved masks.
    outb(master_mask, MASTER_8259_DATA);
    outb(slave_mask, SLAVE_8259_DATA);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    //uint32_t irq_mask = ~(1 << irq_num); // 0 on the (irq_num)th bit

    if (irq_num < 8) { // 8: IMR 8 bits wide
        port = MASTER_8259_DATA;
        value = master_mask & ~(1 << irq_num);
        master_mask = value; // update master_mask
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= 8; // 8: IMR 8 bits wide
        value = slave_mask & ~(1 << irq_num);
        slave_mask = value; // update slave_mask
    }
    outb(value, port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint8_t value;
    //uint32_t irq_mask = 1 << irq_num; // 1 on the (irq_num)th bit

    if (irq_num < 8) { // 8: IMR 8 bits wide
        port = MASTER_8259_DATA;
        value = master_mask | 1 << irq_num;
        master_mask = value; // update master_mask
    } else {
        port = SLAVE_8259_DATA;
        irq_num -= 8; // 8: IMR 8 bits wide
        value = slave_mask | 1 << irq_num;
        slave_mask = value; // update slave_mask
    }
    outb(value, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num >= 8) 
    { // 8: IMR 8 bits wide
        irq_num -= 8;
        outb(EOI | irq_num, SLAVE_8259_PORT); // 8: IMR 8 bits wide
        outb(EOI | 0x2, MASTER_8259_PORT); // 0b010 : CAS = 010
    }
    outb(EOI | irq_num, MASTER_8259_PORT);
}
