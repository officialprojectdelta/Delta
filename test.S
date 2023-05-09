@t = dso_local global double 0x3ff0000000000000, align 8

define dso_local i32 @test() {
    %1 = alloca i32, align 4
store i323, ptr %1 align 4
    %2 = load i32, ptr %1, align 4
store i324, ptr %1 align 4
    %3 = alloca ptr, align 8
    %4 = load i32, ptr %1, align 4
store ptr%1, ptr %3 align 8
    %5 = load ptr, ptr %3, align 8
    %6 = load i32, ptr %5
store i323, ptr %5 align 4
    %7 = alloca i32, align 4
    %8 = call i32 @spread(i32 3)
    %9 = load double, ptr @t, align 8
    %10 = sitofp i32 %8 to double
    %11 = fadd double %10, %9
    %12 = call float @test1()
    %13 = fpext float %12 to double
    %14 = fadd double %11, %13
    %15 = fptosi double %14 to i32
store i32%15, ptr %7 align 4
    %16 = load i32, ptr %7, align 4
    %17 = load double, ptr @t, align 8
    %18 = sitofp i32 %16 to double
    %19 = fadd double %18, %17
    %20 = fsub double %19, 0x4020000000000000
    %21 = fptosi double %20 to i32
    ret i32 %21
}

define dso_local i32 @spread(i32 %0) {
    %2 = alloca i32, align 4
store i32%0, ptr %2 align 4
    %3 = alloca i32, align 4
store i320, ptr %3 align 4
    br label %4

4:
    %5 = load i32, ptr %3, align 4
    %6 = icmp slt i32 %5, 4
    br i1 %6, label %7, label %16

7:
    %8 = load i32, ptr %2, align 4
    %9 = load i32, ptr %2, align 4
    %10 = load i32, ptr %3, align 4
    %11 = sdiv i32 %10, 4
    %12 = add i32 %9, %11
store i32%12, ptr %2 align 4
    br label %13

13:
    %14 = load i32, ptr %3, align 4
    %15 = add i32 %14, 1
store i32%15, ptr %3 align 4
    br label %4

16:
    %17 = alloca float, align 4
store float0x4002666660000000, ptr %17 align 4
    %18 = alloca i32, align 4
    %19 = load float, ptr %17, align 4
    %20 = fcmp une float %19, 0.000000e+00
    %21 = xor i1 %20, true
    %22 = zext i1 %21 to i32
store i32%22, ptr %18 align 4
    %23 = load i32, ptr %18, align 4
    %24 = load float, ptr %17, align 4
    %25 = fneg float%24
    %26 = fptosi float %25 to i32
store i32%26, ptr %18 align 4
    %27 = alloca i32, align 4
    %28 = load i32, ptr %18, align 4
    %29 = add i32 %28, 1
store i32%29, ptr %18 align 4
    %30 = add i32 %28, 3
store i32%30, ptr %27 align 4
    %31 = alloca i16, align 2
    %32 = load i32, ptr %27, align 4
    %33 = trunc i32 %32 to i16
store i16%33, ptr %31 align 2
    %34 = load i32, ptr %2, align 4
    ret i32 %34
}

define dso_local ptr @blah(ptr %0) {
    %2 = alloca ptr, align 8
store ptr%0, ptr %2 align 8
    %3 = alloca ptr, align 8
    %4 = load ptr, ptr %2, align 8
store ptr%2, ptr %3 align 8
    %5 = alloca i32, align 4
    %6 = load ptr, ptr %2, align 8
    %7 = load i32, ptr %6
    %8 = call i32 @spread(i32 %7)
store i32%8, ptr %5 align 4
    %9 = load ptr, ptr %2, align 8
    %10 = load i32, ptr %5, align 4
store ptr%5, ptr %2 align 8
    %11 = load ptr, ptr %2, align 8
    ret ptr %11
}

define dso_local float @test1() {
    %1 = alloca float, align 4
store float0x4023a56040000000, ptr %1 align 4
    %2 = load float, ptr %1, align 4
    %3 = load float, ptr %1, align 4
    %4 = fadd float %3, 0x4020000000000000
store float%4, ptr %1 align 4
    %5 = load float, ptr %1, align 4
    %6 = load float, ptr %1, align 4
    %7 = fpext float %6 to double
    %8 = fadd double %7, 0x4007333333333333
    %9 = fptrunc double %8 to float
store float%9, ptr %1 align 4
    %10 = alloca i32, align 4
store i3223, ptr %10 align 4
    %11 = load double, ptr @t, align 8
store double0x4022000000000000, ptr @t align 8
    %12 = load float, ptr %1, align 4
    %13 = load i32, ptr %10, align 4
    %14 = sitofp i32 %13 to float
    %15 = fadd float %12, %14
    ret float %15
}

