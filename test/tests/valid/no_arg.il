.text
global def i32 threefun ():
    mov i32 %i0, i32 3
    ret i32 %i0

global def i32 mainfun ():
    call i32 %i0, threefun()
    ret i32 %i0

