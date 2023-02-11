.text

#define INTR_LINK(linkage, handler) \
.global linkage ;\
linkage: ;\
    pushfl ;\
    pushal  ;\
    call handler ;\
    popal ;\
    popfl ;\
    iret

INTR_LINK(divide_zero_linkage, divide_zero_handler)
INTR_LINK(single_step_linkage, single_step_handler)
INTR_LINK(nmi_linkage, nmi_handler)
INTR_LINK(breakpoint_linkage, breakpoint_handler)
INTR_LINK(overflow_linkage, overflow_handler)
INTR_LINK(bound_range_linkage, bound_range_handler)
INTR_LINK(invalid_opcode_linkage, invalid_opcode_handler)
INTR_LINK(coprocessor_missing_linkage, coprocessor_missing_handler)
INTR_LINK(double_fault_linkage, double_fault_handler)
INTR_LINK(coprocessor_seg_linkage, coprocessor_seg_handler)
INTR_LINK(invalid_task_linkage, invalid_task_handler)
INTR_LINK(segment_missing_linkage, segment_missing_handler)
INTR_LINK(stack_segment_fault_linkage, stack_segment_fault_handler)
INTR_LINK(general_protection_fault_linkage, general_protection_fault_handler)
INTR_LINK(page_fault_linkage, page_fault_handler)
INTR_LINK(x87_floating_point_linkage, x87_floating_point_handler)
INTR_LINK(alignment_check_linkage, alignment_check_handler)
INTR_LINK(machine_check_linkage, machine_check_handler)
INTR_LINK(simd_floating_point_linkage, simd_floating_point_handler)
INTR_LINK(virtualization_linkage, virtualization_handler)
INTR_LINK(control_protection_linkage, control_protection_handler)
INTR_LINK(hypervisor_injection_linkage, hypervisor_injection_handler)
INTR_LINK(vmm_communication_linkage, vmm_communication_handler)
INTR_LINK(security_linkage, security_handler)


INTR_LINK(rtc_linkage, rtc_handler)
INTR_LINK(keyboard_linkage, keyboard_handler)
