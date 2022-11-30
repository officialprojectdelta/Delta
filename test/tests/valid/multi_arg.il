.text
global def i32 sub_3fun (i32 &sub_3funxfvar, i32 &sub_3funyfvar, i32 &sub_3funzfvar):
    mov i32 %i0, i32 &sub_3funxfvar
    mov i32 %i1, i32 &sub_3funyfvar
    sub i32 %i0, i32 %i0, i32 %i1
    mov i32 %i1, i32 &sub_3funzfvar
    sub i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

global def i32 mainfun ():
    mov i32 %i0, i32 10
    mov i32 %i1, i32 4
    mov i32 %i2, i32 2
    call i32 %i0, sub_3fun(i32 %i0, i32 %i1, i32 %i2)
    ret i32 %i2

