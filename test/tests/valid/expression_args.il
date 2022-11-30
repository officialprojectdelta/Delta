.text
global def i32 addfun (i32 &addfunafvar, i32 &addfunbfvar):
    mov i32 %i0, i32 &addfunafvar
    mov i32 %i1, i32 &addfunbfvar
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 1
    mov i32 %i1, i32 2
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 4
    call i32 %i0, addfun(i32 %i0, i32 %i1)
    def i32 &sum1, i32 %i1
    mov i32 %i1, i32 &sum1
    mov i32 %i2, i32 &sum1
    add i32 %i1, i32 %i1, i32 %i2
    ret i32 %i1

