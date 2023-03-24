.text
global def i32 mainfun ():
    mov i32 %i0, i32 2
    mov i32 %i1, i32 1
    setl i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

