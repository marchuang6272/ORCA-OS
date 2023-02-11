.text
.global pit_linkage 
pit_linkage:
    cli
    pushl %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx 
    pushl %ebx
    pushfl
    pushl %esp
    call save_registers_esp
    addl $4, %esp
    call pit_handler
    call restore_register_esp
    movl %eax, %esp
    popfl 
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp  
    sti
    iret

.global terminal_transition
terminal_transition:
    call restore_register_esp_terminal
    movl %eax, %esp
    popfl 
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    sti
    iret 