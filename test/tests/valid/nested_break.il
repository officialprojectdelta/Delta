.text
global def i32 mainfun ():
    mov i32 %i0, i32 0
    def i32 &ans0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &i1, i32 %i0
.LC0:
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 10
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC1, i32 0, i32 %i0
    mov i32 %i0, i32 0
    def i32 &j2, i32 %i0
.LC3:
    mov i32 %i0, i32 &j2
    mov i32 %i1, i32 10
    setl i32 %i0, i32 %i0, i32 %i1
    je .LC4, i32 0, i32 %i0
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 2
    div i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 2
    mul i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 &i1
    sete i32 %i0, i32 %i0, i32 %i1
    je .LC6 i32 0, i32 %i0
    jmp .LC4
    jmp .LC7
.LC6:
    mov i32 %i0, i32 &ans0
    mov i32 %i1, i32 &i1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &ans0, i32 %i0
.LC7:
.LC5:
    mov i32 %i0, i32 &j2
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &j2, i32 %i0
    jmp .LC3
.LC4:
.LC2:
    mov i32 %i0, i32 &i1
    mov i32 %i1, i32 1
    add i32 %i0, i32 %i0, i32 %i1
    mov i32 &i1, i32 %i0
    jmp .LC0
.LC1:
    mov i32 %i0, i32 &ans0
    ret i32 %i0

