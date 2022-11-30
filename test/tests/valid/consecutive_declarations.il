.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &a0, i32 %i0
    mov i32 %i0, i32 1
    def i32 &b1, i32 %i0
    mov i32 %i0, i32 &b1
    mov i32 &a0, i32 %i0
    mov i32 %i0, i32 2
    def i32 &b2, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 &b2
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    ret i32 %i0

