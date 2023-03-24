.text
global def i32 mainfun ():
    mov i32 %i0, i32 5
    call i32 %i0, foofun(i32 %i0)
    ret i32 %i0

global def i32 foofun (i32 &foofunafvar):
    mov i32 %i0, i32 &foofunafvar
    mov i32 %i1, i32 0
    setle i32 %i0, i32 %i0, i32 %i1
    je .LC0 i32 0, i32 %i0
    mov i32 %i0, i32 &foofunafvar
    ret i32 %i0
.LC0:
    mov i32 %i0, i32 &foofunafvar
    mov i32 %i1, i32 &foofunafvar
    mov i32 %i2, i32 1
    sub i32 %i1, i32 %i1, i32 %i2
    call i32 %i1, barfun(i32 %i1, )
    add i32 %i1, i32 %i1, i32 %i2
    ret i32 %i1

global def i32 barfun (i32 &barfunbfvar):
    mov i32 %i1, i32 &barfunbfvar
    mov i32 %i2, i32 0
    setle i32 %i1, i32 %i1, i32 %i2
    je .LC1 i32 0, i32 %i1
    mov i32 %i1, i32 &barfunbfvar
    ret i32 %i1
.LC1:
    mov i32 %i1, i32 &barfunbfvar
    mov i32 %i2, i32 &barfunbfvar
    mov i32 %i3, i32 2
    div i32 %i2, i32 %i2, i32 %i3
    call i32 %i2, barfun(i32 %i2, )
    add i32 %i2, i32 %i2, i32 %i3
    ret i32 %i2

