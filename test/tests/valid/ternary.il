.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 1
    neg i32 %i1, i32 %i1
    setg i32 %i0, i32 %i0, i32 %i1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 4
    jmp .LC1
.LC0:
    mov i32 %i0, i32 5
.LC1:
    ret i32 %i0

