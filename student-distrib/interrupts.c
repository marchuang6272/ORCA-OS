#include "interrupts.h"
#include "lib.h"

// Vector 0
void divide_zero_handler(void)
{
    printf("Exception: Divide by Zero");
    while(1);
}

// Vector 1
void single_step_handler(void)
{
    printf("Exception: Single-step interrupt");
    while(1);
}

// Vector 2
void nmi_handler(void)
{
    printf("Exception: NMI Exception");
    while(1);
}

// Vector 3
void breakpoint_handler(void)
{
    printf("Exception: Breakpoint Exception");
    while(1);
}

// Vector 4
void overflow_handler(void)
{
    printf("Exception: Overflow Exception");
    while(1);
}

// Vector 5
void bound_range_handler(void)
{
    printf("Exception: Bound Range exceeded");
    while(1);
}

// Vector 6
void invalid_opcode_handler(void)
{
    printf("Exception: Invalid Opcode");
    while(1);
}

// Vector 7
void coprocessor_missing_handler(void)
{
    printf("Exception: Coprocessor not available");
    while(1);
}

// Vector 8
void double_fault_handler(void)
{
    printf("Exception: Double Fault Exception");
    while(1);
}

// Vector 9
void coprocessor_seg_handler(void)
{
    printf("Exception: Coprocessor Segment Overrun");
    while(1);
}

// Vector 0xA
void invalid_task_handler(void)
{
    printf("Exception: Invalid Task State Segment");
    while(1);
}

// Vector 0xB
void segment_missing_handler(void)
{
    printf("Exception: Segment Not Present");
    while(1);
}

// Vector 0xC
void stack_segment_fault_handler(void)
{
    printf("Exception: Stack-Segment Fault");
    while(1);
}

// Vector 0xD
void general_protection_fault_handler(void)
{
    printf("Exception: General Protection Fault");
    while(1);
}

// Vector 0xE
void page_fault_handler(void)
{
    printf("Exception: Page Fault");
    while(1);
}

// Vector 0x10
void x87_floating_point_handler(void)
{
    printf("Exception: x87 Floating-Point Exception");
    while(1);
}

// Vector 0x11
void alignment_check_handler(void)
{
    printf("Exception: Alignment Check");
    while(1);
}

// Vector 0x12
void machine_check_handler(void)
{
    printf("Exception: Machine Check");
    while(1);
}

// Vector 0x13
void simd_floating_point_handler(void)
{
    printf("Exception: SIMD Floating-Point Exception");
    while(1);
}

// Vector 0x14
void virtualization_handler(void)
{
    printf("Exception: Virtualization Exception");
    while(1);
}

// Vector 0x15
void control_protection_handler(void)
{
    printf("Exception: Control Protection Exception");
    while(1);
}

// Vector 0x1C
void hypervisor_injection_handler(void)
{
    printf("Exception: Hypervisor Injection Exception");
    while(1);
}

// Vector 0x1D
void vmm_communication_handler(void)
{
    printf("Exception: VMM Communication Exception");
    while(1);
}

// Vector 0x1E
void security_handler(void)
{
    printf("Exception: Security Exception");
    while(1);
}
