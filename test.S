.globl test
test:
    mov $3, %eax
    neg %eax
    cmpl $0, %eax
    je .LC0
    mov $1, %eax
    cmpl $0, %eax
    mov $0, %eax
    setne %al
.LC0:
    ret