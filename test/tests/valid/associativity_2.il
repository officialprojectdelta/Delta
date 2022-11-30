.text
global def i32 mainfun ():
    mov i32 %i0, i32 6
    mov i32 %i1, i32 3
    div i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 2
    div i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

