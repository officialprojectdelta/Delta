.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &ret0, i32 %i0
    mov i32 %i0, i32 &aglb
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &a1, i32 %i0
    mov i32 %i0, i32 4
    mov i32 &ret0, i32 %i0
.LC0:
    mov i32 %i0, i32 &ret0
    ret i32 %i0

.data
    global def i32 &aglb i32 3
