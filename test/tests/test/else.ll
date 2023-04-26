define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 0, i32* %1, align 4
    %2 = load i32, i32* %1, align 4
    %3 = icmp ne i32 %2, 0
    br i1 %3, label %4, label %5

4:
    ret i32 1

5:
    ret i32 2

6:
    ret i32 0
}

