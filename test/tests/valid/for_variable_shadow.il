.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &i0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &j0, i32 %i0
    mov i32 %i0, i32 0
    mov i32 &i0, i32 %i0
.LC0:
    mov i32 %i0, i32 &i0
    mov i32 %i1, i32 10
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 &i0
    def i32 &k2, i32 %i0
    mov i32 %i0, i32 &k2
    def i32 &i3, i32 %i0
.LC3:
    mov i32 %i0, i32 &i3
    mov i32 %i1, i32 10
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC4, i32 0, i32 %i0
    mov i32 %i0, i32 &j0
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &j0, i32 %i0
.LC5:
    mov i32 %i0, i32 &i3
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &i3, i32 %i0
    jmp .LC3
.LC4:
.LC2:
    mov i32 %i0, i32 &i0
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &i0, i32 %i0
    jmp .LC0
.LC1:
    mov i32 %i0, i32 &j0
    mov i32 %i1, i32 &i0
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

