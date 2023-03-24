.text
global def i32 mainfun ():
    mov i32 %i0, i32 &aglb
    mov i32 %i1, i32 &bglb
    mul i32 %i0, i32 %i0, i32 %i1
    ret i32 %i0

.data
    global def i32 &aglb i32 3
    global def i32 &bglb i32 4
