.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &sum0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &i1, i32 %i0
.LC0:
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 10
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &i1, i32 %i0
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 2
    mod i32 %i0, i32 %i0, i32 %i1
    je .LC3 i32 0, i32 %i0
    jmp .LC2
.LC3:
    mov i32 %i0, i32 &sum0
    mov i32 %i1, i32 &i1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &sum0, i32 %i0
.LC2:
    jmp .LC0
.LC1:
    mov i32 %i0, i32 &sum0
    ret i32 %i0

