.text
global def i32 testfun ():
    mov i32 %i0, i32 4
    def i32 &x0, i32 %i0
    mov i32 %i0, i32 &x0
    mov i32 %i1, i32 7
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC0, i32 %i0 i32 0
    mov i32 %i0, i32 3
    ret i32 %i0
    jmp .LC1
.LC0:
    mov i32 %i0, i32 &x0
    mov i32 %i1, i32 2
    div i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0
.LC1:
ret i32 0

