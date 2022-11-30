.text
global def i32 fibfun (i32 &fibfunnfvar):
    mov i32 %i0, i32 &fibfunnfvar
    mov i32 %i1, i32 0
    sete i32 %i0, i32 %i0, i32 %i1
    sete i32 %i0, i32 0, i32 %i0
    je .LC0, i32 0, i32 %i0
    mov i32 %i0, i32 &fibfunnfvar
    mov i32 %i1, i32 1
    sete i32 %i0, i32 %i0, i32 %i1
    setne i32 %i0, i32 0, i32 %i0
.LC0:
    je .LC1 i32 0, i32 %i0
    mov i32 %i0, i32 &fibfunnfvar
    ret i32 %i0
    jmp .LC2
.LC1:
    mov i32 %i0, i32 &fibfunnfvar
    mov i32 %i1, i32 1
    sub i32 %i0, i32 %i0, i32 %i1
    call i32 %i0, fibfun(i32 %i0)
    mov i32 %i1, i32 &fibfunnfvar
    mov i32 %i2, i32 2
    sub i32 %i1, i32 %i1, i32 %i2
    call i32 %i1, fibfun(i32 %i1, )
    add i32 %i1, i32 %i1, i32 %i2
    ret i32 %i1
.LC2:
ret null

global def i32 mainfun ():
    mov i32 %i1, i32 5
    def i32 &n3, i32 %i1
    mov i32 %i1, i32 &n3
    call i32 %i1, fibfun(i32 %i1, )
    ret i32 %i2

