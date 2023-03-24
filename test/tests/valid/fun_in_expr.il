.text
global def i32 sumfun (i32 &sumfunafvar, i32 &sumfunbfvar):
    mov i32 %i0, i32 &sumfunafvar
    mov i32 %i1, i32 &sumfunbfvar
    add i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 1
    mov i32 %i1, i32 2
    call i32 %i0, sumfun(i32 %i0, i32 %i1)
    mov i32 %i2, i32 1
    mov i32 %i3, i32 2
    call i32 %i2, sumfun(i32 %i2, i32 %i3, )
    mov i32 %i5, i32 2
    div i32 %i4, i32 %i4, i32 %i5
    mov i32 %i5, i32 2
    mul i32 %i4, i32 %i4, i32 %i5
    sub i32 %i3, i32 %i3, i32 %i4
    def i32 &a1, i32 %i3
    mov i32 %i3, i32 2
    mov i32 %i4, i32 3
    mov i32 %i5, i32 4
    call i32 %i4, sumfun(i32 %i4, i32 %i5, )
    mul i32 %i5, i32 %i5, i32 %i6
    mov i32 %i6, i32 1
    mov i32 %i7, i32 2
    call i32 %i6, sumfun(i32 %i6, i32 %i7, )
    add i32 %i7, i32 %i7, i32 %i8
    def i32 &b1, i32 %i7
    mov i32 %i7, i32 &b1
    mov i32 %i8, i32 &a1
    sub i32 %i7, i32 %i7, i32 %i8
    ret i32 %i7

