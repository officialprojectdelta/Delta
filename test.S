.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    movl $1, -4(%rbp)
    movl -4(%rbp), %eax
    cmp $0, %eax
    mov $0, %eax
    sete %al
    cmpl $0, %eax
    je .LC0
    movl $3, -8(%rbp)
    movl $2, %eax
    addl -4(%rbp), %eax
    movl %eax, -8(%rbp)
    jmp .LC1
.LC0:
    movl $2, -4(%rbp)
.LC1:
    movl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
