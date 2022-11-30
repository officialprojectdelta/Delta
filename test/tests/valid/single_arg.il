.text
global def i32 twicefun (i32 &twicefunxfvar):
    mov i32 %i0, i32 2
    mov i32 %i1, i32 &twicefunxfvar
    mul i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 3
    call i32 %i0, twicefun(i32 %i0)
    ret i32 %i0

