.text
global def i32 mainfun ():
.LC0:
    mov i32 %i0, i32 1
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 2
    ret i32 %i0
    jmp .LC0
.LC1:
ret null

