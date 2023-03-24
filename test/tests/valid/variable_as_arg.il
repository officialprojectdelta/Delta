.text
global def i32 foofun (i32 &foofunxfvar):
    mov i32 %i0, i32 &foofunxfvar
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 1
    def i32 &a1, i32 %i0
    mov i32 %i0, i32 &a1
    call i32 %i0, foofun(i32 %i0)
    ret i32 %i0

