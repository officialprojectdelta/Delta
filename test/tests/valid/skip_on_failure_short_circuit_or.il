.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &b0, i32 %i0
    mov i32 %i0, i32 &a0
    sete i32 %i0, i32 0, i32 %i0
    je .LC0, i32 0, i32 %i0
    mov i32 %i0, i32 5
    mov i32 &b0, i32 %i0
    setne i32 %i0, i32 0, i32 %i0
.LC0:
    mov i32 %i0, i32 &b0
    ret i32 %i0

