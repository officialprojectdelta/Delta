.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &i0, i32 %i0
    mov i32 %i0, i32 2
    def i32 &a1, i32 %i0
    mov i32 %i0, i32 3
    def i32 &b0, i32 %i0
    mov i32 %i0, i32 &b0
    ret i32 %i0

