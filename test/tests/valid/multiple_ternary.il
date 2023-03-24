.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    mov i32 %i1, i32 2
    setg i32 %i0, i32 %i0, i32 %i1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 3
    jmp .LC1
.LC0:
    mov i32 %i0, i32 4
.LC1:
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 1
    mov i32 %i1, i32 2
    setg i32 %i0, i32 %i0, i32 %i1
    je .LC2 i32 0, i32 %i0
    mov i32 %i0, i32 5
    jmp .LC3
.LC2:
    mov i32 %i0, i32 6
.LC3:
    def i32 &b0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 &b0
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

