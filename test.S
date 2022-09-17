.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    movl $8, %eax
    subl $8, %eax
    cmpl $0, %eax
    je .LC0
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp .LC1
.LC0:
    movl $5, %eax
    subl $5, %eax
    cmpl $0, %eax
    je .LC2
    movl $3, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    jmp .LC3
.LC2:
    movl $1, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
.LC3:
.LC1:
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
