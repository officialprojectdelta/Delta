.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    def i32 &a0, i32 %i0
.LC0:
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 2
    mul i32 %i0, i32 %i0, i32 %i1
    mov i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 11
    setl i32 %i0, i32 %i0, i32 %i1
.LC1:
    je .LC0, i32 0, i32 %i0
.LC2:
    mov i32 %i0, i32 &a0
    ret i32 %i0

