.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 1
    ret i32 %i0
    jmp .LC1
.LC0:
    mov i32 %i0, i32 2
    ret i32 %i0
.LC1:
ret null

