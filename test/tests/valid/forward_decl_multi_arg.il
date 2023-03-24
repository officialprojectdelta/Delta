.text
global def i32 mainfun ():
    mov i32 %i0, i32 1
    mov i32 %i1, i32 2
    call i32 %i0, foofun(i32 %i0, i32 %i1)
    ret i32 %i1

global def i32 foofun (i32 &foofunxfvar, i32 &foofunyfvar):
    mov i32 %i1, i32 &foofunxfvar
    mov i32 %i2, i32 &foofunyfvar
    sub i32 %i1, i32 %i1, i32 %i2
    ret i32 %i1

