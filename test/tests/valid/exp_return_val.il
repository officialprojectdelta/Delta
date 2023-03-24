.text
global def i32 mainfun ():
    def i32 &a0
    def i32 &b0
    mov i32 %i0, i32 4
    mov i32 &b0, i32 %i0
    mov i32 &a0, i32 %i0
    mov i32 %i0, i32 &a0
    mov i32 %i1, i32 &b0
    sub i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

