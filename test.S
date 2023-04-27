define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 2, i32* %1, align 4
    %2 = load i32, i32* %1, align 4
    %3 = icmp slt i32 %2, 3
    br i1 %3, label %4, label %7

4:
    %5 = alloca i32, align 4
    store i32 3, i32* %5, align 4
    %6 = load i32, i32* %5, align 4
    ret i32 %6

7:
    ret i32 0
}

