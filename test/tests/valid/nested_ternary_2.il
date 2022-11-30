.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 2
    je .LC1 i32 0, i32 %i0
    mov i32 %i0, i32 3
    jmp .LC2
.LC1:
    mov i32 %i0, i32 4
.LC2:
    jmp .LC3
.LC0:
    mov i32 %i0, i32 5
.LC3:
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 0
    je .LC4 i32 0, i32 %i0
    mov i32 %i0, i32 2
    je .LC5 i32 0, i32 %i0
    mov i32 %i0, i32 3
    jmp .LC6
.LC5:
    mov i32 %i0, i32 4
.LC6:
    jmp .LC7
.LC4:
    mov i32 %i0, i32 5
.LC7:
    def i32 &b0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 &b0
    mul i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

