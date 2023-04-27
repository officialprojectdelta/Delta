define dso_local i32 @test() {
    %1 = alloca i32, align 4
    %2 = select i1 1, i1 0, i1 0
    %3 = zext i1 %2 to i32
    store i32 %3, i32* %1, align 4
    ret i32 0
}

