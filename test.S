.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    mov $3, %eax
    movl %eax, -4(%rbp)
    mov $4, %eax
    movl %eax, -8(%rbp)
    mov $7, %eax
    movl %eax, -12(%rbp)
    movl -8(%rbp), %eax
    push %rax
    mov $234, %eax
    imul $2, %eax
    movl %eax, -12(%rbp)
    pop %rcx
    imul %ecx, %eax
    movl %eax, -4(%rbp)
    movl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret