.text
global def i32 testfun():
    mov i32 %i0, i32 3
    ret i32 %i0

global def i32 mainfun():
    mov i32 %i0, i32 0
    setne i32 %i0, i32 0, i32 %i0
    je .LC0, i32 0, i32 %i0
    mov i32 %i0, i32 9
    setne i32 %i0, i32 0, i32 %i0
.LC0:
    def i32 &x1, i32 %i0
    mov i32 %i0, i32 8
    sete i32 %i0, i32 0, i32 %i0
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 9
    setne i32 %i0, i32 0, i32 %i0
.LC1:
    ret i32 %i0

