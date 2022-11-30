.text
global def i32 mainfun ():
    call i32 %i0, foofun()
    ret i32 %i0

global def i32 foofun ():
    mov i32 %i0, i32 3
    ret i32 %i0

