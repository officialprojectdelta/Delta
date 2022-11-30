.text
global def i32 mainfun ():
    mov i32 %i0, i32 2
    mov i32 %i1, i32 2
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 %i0, i32 0
    ret i32 %i0

