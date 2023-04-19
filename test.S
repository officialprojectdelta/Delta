@global = dso_local global i32 42, align 4

@gb2 = dso_local global i8 0x40a91a0000000000, align 1

define dso_local i32 @test1(i32 %0) {
    %2 = alloca i32, align 4
    store i32 %0, i32* %2, align 4
    %3 = load i32, i32* %2, align 4
    %4 = add i32 %3, 4
    ret i32 %4
    ret i32 0
}

define dso_local i32 @test() {
    %1 = alloca i8, align 1
    %2 = sub i32 0, 5
    %3 = trunc i32 %2 to i8
    store i8 %3, i8* %1, align 1
    %4 = alloca i32, align 4
    store i32 4, i32* %4, align 4
    %5 = load i32, i32* %4, align 4
    %6 = load i32, i32* @global, align 4
    store i32 %6, i32* %4, align 4
    %7 = load i32, i32* %4, align 4
    %8 = load i32, i32* %4, align 4
    %9 = load i8, i8* %1, align 1
    %10 = zext i8 %9 to i32
    %11 = add i32 %8, %10
    %12 = call i32 @test1(i32 %11)
    store i32 %12, i32* %4, align 4
    %13 = load i32, i32* %4, align 4
    ret i32 %13
    ret i32 0
}

