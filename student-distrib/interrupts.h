#ifndef _INTERUPTS_H
#define _INTERUPTS_H
/*INTERRUPTS*/
void divide_zero_handler(void); // 0
void single_step_handler(void); // 1
void nmi_handler(void); // 2
void breakpoint_handler(void); // 3
void overflow_handler(void); // 4
void bound_range_handler(void); // 5
void invalid_opcode_handler(void); // 6
void coprocessor_missing_handler(void); // 7
void double_fault_handler(void); // 8
void coprocessor_seg_handler(void); // 9
void invalid_task_handler(void); // 0xA
void segment_missing_handler(void); // 0xB
void stack_segment_fault_handler(void); // 0xC
void general_protection_fault_handler(void); // 0xD
void page_fault_handler(void); // 0xE
void x87_floating_point_handler(void); // 0x10
void alignment_check_handler(void); // 0x11
void machine_check_handler(void); // 0x12
void simd_floating_point_handler(void); // 0x13
void virtualization_handler(void); // 0x14
void control_protection_handler(void); // 0x15
void hypervisor_injection_handler(void); // 0x1C
void vmm_communication_handler(void); // 0x1D
void security_handler(void); // 0x1E
void system_call_handler(void); // 0x80

#endif
/*** end of file*/
