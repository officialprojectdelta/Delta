.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 0
    je .LC1 i32 0, i32 %i0
    mov i32 %i0, i32 3
    mov i32 &a0, i32 %i0
    jmp .LC2
.LC1:
    mov i32 %i0, i32 4
    mov i32 &a0, i32 %i0
.LC2:
.LC0:
    mov i32 %i0, i32 &a0
    ret i32 %i0

