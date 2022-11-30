.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    setne i32 %i0, i32 0, i32 %i0
    je .LC0, i32 0, i32 %i0
    mov i32 %i0, i32 1
    neg i32 %i0, i32 %i0
    setne i32 %i0, i32 0, i32 %i0
.LC0:
    ret i32 %i0

