@global = dso_local global i32 42, align 4

@gb2 = dso_local global i8 -115, align 1

define dso_local i32 @test1(i32 %0) {
    %2 = alloca i32, align 4
    store i32 %0, i32* %2, align 4
    %3 = alloca i8, align 1
    %4 = load i32, i32* %2, align 4
    %5 = trunc i32 %4 to i8
    store i8 %5, i8* %3, align 1
    %6 = load i8, i8* %3, align 1
    %7 = sext i8 %6 to i32
    ret i32 %7
    ret i32 0
}

define dso_local i32 @test() {
    %1 = alloca i64, align 8
    store i64 12312322323, i64* %1, align 8
    %2 = alloca double, align 8
    store double 0x3fceb4fc3d849922, double* %2, align 8
    %3 = alloca i32, align 4
    store i32 -5, i32* %3, align 4
    %4 = alloca i16, align 2
    store i16 -5, i16* %4, align 2
    %5 = alloca i32, align 4
    %6 = load i16, i16* %4, align 2
    %7 = sext i16 %6 to i32
    store i32 %7, i32* %5, align 4
    %8 = load i32, i32* %5, align 4
    %9 = load i32, i32* @global, align 4
    store i32 %9, i32* %5, align 4
    %10 = alloca float, align 4
    store float 0x3f53a92a20000000, float* %10, align 4
    %11 = load i32, i32* %5, align 4
    %12 = zext i32 %11 to i32
    ret i32 %12
    ret i32 0
}

