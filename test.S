define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 0, i32* %1, align 4
    br label %2

2:
    br i1 1, label %3, label %12

3:
    %4 = load i32, i32* %1, align 4
    %5 = load i32, i32* %1, align 4
    %6 = add i32 %5, 1
    store i32 %6, i32* %1, align 4
    %7 = load i32, i32* %1, align 4
    %8 = icmp sgt i32 %7, 3
    br i1 %8, label %9, label %10

9:
    br label %12




10:
    br label %11

11:
    br label %2

12:
    %13 = load i32, i32* %1, align 4
    ret i32 %13

}

