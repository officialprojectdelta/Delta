.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 2
    jmp .LC1
.LC0:
    mov i32 %i0, i32 3
.LC1:
    mov i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    ret i32 %i0

