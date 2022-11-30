.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &b0, i32 %i0
    mov i32 %i0, i32 &a0
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 2
    mov i32 &a0, i32 %i0
    jmp .LC1
.LC0:
    mov i32 %i0, i32 3
    mov i32 &a0, i32 %i0
.LC1:
    mov i32 %i0, i32 &b0
    je .LC2 i32 0, i32 %i0
    mov i32 %i0, i32 4
    mov i32 &b0, i32 %i0
    jmp .LC3
.LC2:
    mov i32 %i0, i32 5
    mov i32 &b0, i32 %i0
.LC3:
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 &b0
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

