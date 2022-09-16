.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    movl $3, -4(%rbp)
    movl $8, %eax
    subl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret