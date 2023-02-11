
.globl loadPageDirectory
loadPageDirectory:
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    movl 8(%ebp), %eax
    movl %eax, %cr3
    popl %edi
    popl %esi
    popl %ebx
    leave
    ret

.globl enablePaging
enablePaging:
    pushl %ebp
    movl %esp, %ebp
    pushl %ebx
    pushl %esi
    pushl %edi
    movl %cr4, %ebx       
    orl  $0x00000010, %ebx 
    mov %ebx, %cr4       
    movl %cr0, %ebx       
    orl  $0x80000000, %ebx 
    movl %ebx, %cr0   
    popl %edi
    popl %esi
    popl %ebx
    leave
    ret

.global flush_tlb
flush_tlb: 
    movl %cr3, %eax
    movl %eax, %cr3
    ret
