.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    not i32 %i0, i32 %i0
    ret i32 %i0

