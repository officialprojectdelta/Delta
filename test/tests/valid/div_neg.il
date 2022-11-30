.text
global def i32 mainfun ():
    mov i32 %i0, i32 12
    neg i32 %i0, i32 %i0
    mov i32 %i1, i32 5
    div i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

