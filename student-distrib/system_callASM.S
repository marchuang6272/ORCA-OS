.text

# main system call assembly linkage function
.global system_call_linkage
system_call_linkage: 
    cli
    cmpl $0, %eax
    jle invalid_eax
    cmpl $10, %eax
    jg invalid_eax
    pushl %ebp
    movl %esp, %ebp
    pushl %edi
    pushl %esi
    pushl %edx
    pushl %ecx 
    pushl %ebx
    call *system_call_table-4(, %eax, 4)
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp   # restores ebp
    sti
    iret
    invalid_eax:
    movl $-1, %eax
    sti 
    iret
    
# Wrapper function for sys_halt
system_halt: 
    popl %edi
    call restore_esp
    movl %eax, %esp     # restores esp
    call check_process_num # function that returns whether we can halt or not
    cmpl $3, %eax # program 0, 1, and 2 cannot be exited from. halt will have 3, so check if greater than 3 for valid
    jg valid_halt
    call start_first_shell
    # jmp *%edi
    valid_halt:
    call sys_halt  
    xorl %eax, %eax # clear eax
    movb %bl, %al # move lowest byte into EAX
    jmp *%edi

# Execute assembly wrapper
.global system_execute
system_execute:
    sti
    popl %edi
    call sys_execute
    cmpl $-1, %eax      # checks for error 
    jne continue_execute
    jmp *%edi
    continue_execute:
    pushl %eax          # saves eip on stack
    pushl %esp
    call save_esp
    popl %esp
    popl %eax           # restores eip
    pushl $0x002B       # offset of user stack segment in GDT (SS)
    pushl $0x083FFFFC   # address of user stack (ESP)
    pushfl              # push flags register (EFLAGS)
    pushl $0x0023       # offset for user code segment in GDT (CS)
    pushl %eax          # pointer to user instruction (EIP)
    movw $0x002B, %cx   # updates user data segment register (DS)+
    movw %cx, %ds  
    sti
    iret

# Below are wrappers for the system calls

# Reads data from the keyboard, a file, device (RTC), or directory
system_read:
    cli
    popl %edi
    call sys_read
    jmp *%edi

# Writes data to the terminal or to a device (RTC)
system_write:
    cli
    popl %edi
    call sys_write
    sti
    jmp *%edi

# Provides access to the file system. The call should find the directory entry corresponding to the
# named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
# RTC device, or regular file)
system_open:
    popl %edi
    call sys_open
    jmp *%edi

# Call closes the specified file descriptor and makes it available for return from later calls to open
system_close:
    popl %edi
    call sys_close
    jmp *%edi

# Reads the program’s command line arguments into a user-level buffer
system_getargs:
    popl %edi
    call sys_getargs
    jmp *%edi

# Maps the text-mode video memory into user space at a pre-set virtual address
system_vidmap:
    popl %edi
    call sys_vidmap
    jmp *%edi

system_set_handler:
    popl %edi
    call sys_set_handler
    jmp *%edi

system_sigreturn:
    popl %edi
    call sys_sigreturn
    jmp *%edi

# jump table for system calls
system_call_table:
    .long system_halt
    .long system_execute
    .long system_read
    .long system_write
    .long system_open
    .long system_close
    .long system_getargs
    .long system_vidmap 
    .long system_set_handler 
    .long system_sigreturn

