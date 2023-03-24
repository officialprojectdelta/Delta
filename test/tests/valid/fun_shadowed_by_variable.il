.text
global def i32 foofun ():
    mov i32 %i0, i32 3
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 5
    def i32 &foo1, i32 %i0
    mov i32 %i0, i32 &foo1
    ret i32 %i0

