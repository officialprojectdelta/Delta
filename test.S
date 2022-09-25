.globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    movl $3, 24(%rbp)
    movl 16(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret

.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -4(%rbp)
    movl $0, -8(%rbp)
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
    movl -4(%rbp), %eax
    incl -4(%rbp)
    movl %eax, -8(%rbp)
    jmp .LC0
.LC1:
    movl $4, %eax
    pushq %rax
    movl $1, %eax
    pushq %rax
    call foo
    add $16, %rsp
    movq %rbp, %rsp
    popq %rbp
    ret

