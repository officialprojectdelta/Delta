define dso_local i32 @test() {
    %1 = sdiv i8 4, 2
    %2 = sext i8 %1 to i32
    ret i32 %2
    ret i32 0
}

