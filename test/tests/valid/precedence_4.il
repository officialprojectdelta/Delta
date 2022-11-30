.text
global def i32 mainfun ():
    mov i32 %i0, i32 2
    mov i32 %i1, i32 2
    sete i32 %i0, i32 %i0, i32 %i1
    sete i32 %i0, i32 0, i32 %i0
    je .LC0, i32 0, i32 %i0
    mov i32 %i0, i32 0
    setne i32 %i0, i32 0, i32 %i0
.LC0:
    ret i32 %i0

