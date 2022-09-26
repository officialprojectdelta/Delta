.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    movl $0, -4(%rbp)
    movl $3, -8(%rbp)
    movl -4(%rbp), -4(%rbp)
    movl -4(%rbp), %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

