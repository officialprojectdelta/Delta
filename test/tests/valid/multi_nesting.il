.text
global def i32 mainfun ():
    mov i32 %i0, i32 2
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 3
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 3
    def i32 &a2, i32 %i0
    mov i32 %i0, i32 &a2
    ret i32 %i0
    mov i32 %i0, i32 &a0
    ret i32 %i0
.LC0:
ret null

