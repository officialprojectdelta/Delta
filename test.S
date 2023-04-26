define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 1, i32* %1, align 4
    %2 = alloca i32, align 4
    store i32 1, i32* %2, align 4
    %3 = alloca i32, align 4
    store i32 1, i32* %3, align 4
    %4 = load i32, i32* %1, align 4
    %5 = load i32, i32* %2, align 4
    %6 = load i32, i32* %3, align 4
    ret i32 %6
    ret i32 0
}

