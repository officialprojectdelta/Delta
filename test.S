.text
.globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    movl $3, %eax
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

.globl test
test:
    pushq %rbp
    movq %rsp, %rbp
    call foo
    movq %rbp, %rsp
    popq %rbp
    ret
    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

.align 8
.bss
.align 8
.data
.globl fooglb
fooglb:
    .quad 4

