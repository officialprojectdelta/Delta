.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -4(%rbp)
.LC0:
    movl $1, %eax
    cmpl $0, %eax
    je .LC1
    movl $5, %eax
    cmpl -4(%rbp), %eax
    mov $0, %eax
    sete %al
    cmpl $0, %eax
    je .LC2
    jmp .LC1
.LC2:
    movl $1, %eax
    addl -4(%rbp), %eax
    movl %eax, -4(%rbp)
    jmp .LC0
.LC1:
    movl $7, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
