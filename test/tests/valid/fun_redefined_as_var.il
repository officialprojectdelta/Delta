.text
global def i32 foofun ():
    mov i32 %i0, i32 3
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 &fooglb
    ret i32 %i0

.data
    global def i32 &fooglb i32 4
