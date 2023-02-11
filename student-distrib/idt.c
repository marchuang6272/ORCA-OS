/* idt.c - Defines used in interactions with the keyboard
 * vim:ts=4 noexpandtab
 */

#include "idt.h"
#include "interrupts.h"
#include "x86_desc.h"
#include "lib.h"

// idt helper function
void set_idt(uint8_t i) {
    idt[i].present = 0x1; // must be 1 to work
    // Set the gate bits to 0xE (0b1110)
    idt[i].size = 0x1; // 32 bits
    idt[i].reserved1 = 0x1; // interrupt gate: 1110
    idt[i].reserved2 = 0x1;
    idt[i].reserved3 = 0x0;
    if(i == 0x80) 
    {
        idt[i].dpl = 0x3;
    }
    idt[i].seg_selector = KERNEL_CS; 
}

void idt_init() {
    // Initialize first 10 exceptions (their function pointers)
    extern void divide_zero_linkage(void);
    SET_IDT_ENTRY(idt[0], divide_zero_linkage);
    set_idt(0);

    extern void single_step_linkage(void);
    SET_IDT_ENTRY(idt[1], single_step_linkage);
    set_idt(1);

    extern void nmi_linkage(void);
    SET_IDT_ENTRY(idt[2], nmi_linkage);
    set_idt(2);
    
    extern void breakpoint_linkage(void);
    SET_IDT_ENTRY(idt[3], breakpoint_linkage);
    set_idt(3);

    extern void overflow_linkage(void);
    SET_IDT_ENTRY(idt[4], overflow_linkage);
    set_idt(4);

    extern void bound_range_linkage(void);
    SET_IDT_ENTRY(idt[5], bound_range_linkage);
    set_idt(5);

    extern void invalid_opcode_linkage(void);
    SET_IDT_ENTRY(idt[6], invalid_opcode_linkage);
    set_idt(6);

    extern void coprocessor_missing_linkage(void);
    SET_IDT_ENTRY(idt[7], coprocessor_missing_linkage);
    set_idt(7);

    extern void double_fault_linkage(void);
    SET_IDT_ENTRY(idt[8], double_fault_linkage);
    set_idt(8);

    extern void coprocessor_seg_linkage(void);
    SET_IDT_ENTRY(idt[9], coprocessor_seg_linkage);
    set_idt(9);

    extern void invalid_task_linkage(void);
    SET_IDT_ENTRY(idt[0xA], invalid_task_linkage);
    set_idt(0xA);

    extern void segment_missing_linkage(void);
    SET_IDT_ENTRY(idt[0xB], segment_missing_linkage);
    set_idt(0xB);

    extern void stack_segment_fault_linkage(void);
    SET_IDT_ENTRY(idt[0xC], stack_segment_fault_linkage);
    set_idt(0xC);

    extern void general_protection_fault_linkage(void);
    SET_IDT_ENTRY(idt[0xD], general_protection_fault_linkage);
    set_idt(0xD);
    
    extern void page_fault_linkage(void);
    SET_IDT_ENTRY(idt[0xE], page_fault_linkage);
    set_idt(0xE);

    extern void x87_floating_point_linkage(void);
    SET_IDT_ENTRY(idt[0x10], x87_floating_point_linkage);
    set_idt(0x10);

    extern void alignment_check_linkage(void);
    SET_IDT_ENTRY(idt[0x11], alignment_check_linkage);
    set_idt(0x11);

    extern void machine_check_linkage(void);
    SET_IDT_ENTRY(idt[0x12], machine_check_linkage);
    set_idt(0x12);

    extern void simd_floating_point_linkage(void);
    SET_IDT_ENTRY(idt[0x13], simd_floating_point_linkage);
    set_idt(0x13);

    extern void virtualization_linkage(void);
    SET_IDT_ENTRY(idt[0x14], virtualization_linkage);
    set_idt(0x14);

    extern void control_protection_linkage(void);
    SET_IDT_ENTRY(idt[0x15], control_protection_linkage);
    set_idt(0x15);

    extern void hypervisor_injection_linkage(void);
    SET_IDT_ENTRY(idt[0x1C], hypervisor_injection_linkage);
    set_idt(0x1C);

    extern void vmm_communication_linkage(void);
    SET_IDT_ENTRY(idt[0x1D], vmm_communication_linkage);
    set_idt(0x1D);

    extern void security_linkage(void);
    SET_IDT_ENTRY(idt[0x1E], security_linkage);
    set_idt(0x1E);

    extern void system_call_linkage(void);
    SET_IDT_ENTRY(idt[0x80], system_call_linkage);
    set_idt(0x80);
}

