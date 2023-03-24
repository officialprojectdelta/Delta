.text
global def i32 mainfun ():
    mov i32 %i0, i32 &fooglb
    mov i32 %i1, i32 3
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

.data
    global def i32 &fooglb i32 4
