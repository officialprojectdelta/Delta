.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &i0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &j0, i32 %i0
    mov i32 %i0, i32 100
    def i32 &i1, i32 %i0
.LC0:
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 0
    setg i32 %i0, i32 %i0, i32 %i1
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &i2, i32 %i0
    mov i32 %i0, i32 &j2
    mov i32 %i1, i32 2
    mul i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 &i2
    add i32 %i0, i32 %i0, i32 %i1
    def i32 &j2, i32 %i0
.LC2:
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 1
    sub i32 %i0, i32 %i0, i32 %i1
    mov i32 &i1, i32 %i0
    jmp .LC0
.LC1:
    mov i32 %i0, i32 3
    def i32 &k0, i32 %i0
    mov i32 %i0, i32 &j0
    mov i32 %i1, i32 &k0
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

