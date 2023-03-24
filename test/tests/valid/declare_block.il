.text
global def i32 mainfun ():
    mov i32 %i0, i32 5
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &i1, i32 %i0
    mov i32 %i0, i32 &i1
    ret i32 %i0
.LC0:
ret null

