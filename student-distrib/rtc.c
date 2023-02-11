/*
Initializes the RTC by enabling the RTC IRQ on IRQ 8
Must have interrupts disabled, and must install IRQ handler before calling this
Sets the initial frequency to 1024 Hz, this can be changed
*/

#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "idt.h"
#include "scheduling.h"

#define RTC_ADDRESS_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_IRQ_NUMBER 8

volatile int8_t rtc_interrupt_flag = 0;
int rtc_virtualization_counter = 0; //RTC interupt loops from 0 to 1023
int frequency_division_rate[3]; //virtualization checks for a specific modulus of the virtualization_counter
//uint8_t rtc_counter = 0; // for rtc_test

/* Initialize the keyboard */
void rtc_init() {
    cli();                  // disable interrupts
    extern void rtc_linkage(void);
    SET_IDT_ENTRY(idt[0x28], rtc_linkage);
    set_idt(0x28);
    enable_irq(8);

    outb(0x8A, RTC_ADDRESS_PORT);
    uint32_t prev_ = inb(RTC_DATA_PORT);
    outb(0x8A, RTC_ADDRESS_PORT);
    outb((unsigned char)(prev_ & 0xF0) | RTC_FREQ_2, RTC_DATA_PORT); // initialize rtc frequency to 2Hz

    outb(0x8B, RTC_ADDRESS_PORT);		// select register B, and disable NMI
    uint32_t prev = inb(RTC_DATA_PORT);	// read the current value of register B
    outb(0x8B, RTC_ADDRESS_PORT);		// set the index again (a read will reset the index to register D)
    outb((unsigned char)(prev | 0x40), RTC_DATA_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    rtc_interrupt_flag = 0;
    rtc_virtualization_counter = 0;
    frequency_division_rate[0] = 512; //for all terminals, RTC is initialized to 1024 Hz
    frequency_division_rate[1] = 512;
    frequency_division_rate[2] = 512;
    //test_frequency_flag = -1; // dont print unless test has been called

}

// handle rtc interrupts
void rtc_handler()
{
    uint32_t update_in_progress;
    //Read bit 7 of the RTC's status register A
    outb(0x0A, RTC_ADDRESS_PORT);
    update_in_progress = inb(RTC_DATA_PORT) & 0x80;
    //Only read the second if the update_in_progress bit is low

    //Read from register C to enable interrupts to happen again
    outb(0x0C,RTC_ADDRESS_PORT);	// select register C
    inb(RTC_DATA_PORT);		// just throw away contents
    // send EOI
    send_eoi(RTC_IRQ_NUMBER);
    rtc_virtualization_counter++;
    rtc_virtualization_counter = rtc_virtualization_counter % 1024; //RTC counter loops from 0 to 1023

    //frequency_division_rate changes how often this if statement runs
    if ((rtc_virtualization_counter % frequency_division_rate[terminal_on_display]) == 0) { 
        rtc_interrupt_flag = 1; //Breaks out of rtc_read
    }
}

// rtc helper function: change the rtc frequency to "freq"
int32_t rtc_set_freq(int32_t freq) {
    uint32_t rate;
    uint32_t temp = freq >> 1; // binary freq shifts right one bit
    int32_t i = 0;

    outb(0x8A, RTC_ADDRESS_PORT);
    uint32_t prev = inb(RTC_DATA_PORT);
    
    if (!(freq && (!(freq & (freq - 1)))) | (freq > 1024)) { // if not power of two or freq greater than 1024
        return -1;
    }
    while(temp != 0) { // loop through all bits to find the nonzero bit index
        temp = temp >> 1;
        i++;
    }
    rate = 0x0F - (i-1) * 1; // calculate rate
    
    outb(0x8A, RTC_ADDRESS_PORT);
    outb((unsigned char)(prev & 0xF0) | rate, RTC_DATA_PORT);
    return 0;
}

// initializes RTC frequency to 2HZ, return 0
int32_t rtc_open(uint8_t* file_name) {
    rtc_init();
    rtc_set_freq(1024);
    return 0;
}

// probably does nothing, unless you virtualize RTC, return 0
int32_t rtc_close(int32_t fd) {
    rtc_set_freq(1024);
    return 0;
}

// block until the next interrupt, return 0
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) 
{
    sti();
    rtc_interrupt_flag = 0;
    while(1) {
        if (rtc_interrupt_flag) 
        {
            break;
        }
    }
    return 0;
}

// change frequency, return 0, or -1 if invalid frequency
int32_t rtc_write(int32_t fd, void* buf, int32_t nbytes) {
    if (buf == NULL) {
        return -1;
    }
    uint32_t freq = *((uint32_t*)buf);
    return rtc_set_virtualized_freq(freq);
}

int32_t rtc_set_virtualized_freq(int32_t freq) {
    if (freq == 512 || freq == 256 || freq == 128 || freq == 64|| freq == 32 || freq == 16 || freq == 8 || freq == 4 || freq == 2 ) {
        //change the RTC rate
        frequency_division_rate[terminal_on_display] = 1024/freq;
        return 0;
    } else {
        //invalid rate
        return -1;
    }
}

