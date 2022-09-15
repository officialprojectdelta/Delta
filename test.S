.globl test
test:
    mov $9, %eax
    push %rax
    mov $3, %ecx
    pop %rax
    cdq
    idiv %ecx
    ret