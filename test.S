define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 4, ptr %1, align 4
    %2 = alloca ptr, align 8
    store ptr null, ptr %2, align 8
    %3 = alloca ptr, align 8
    %4 = load ptr, ptr %2, align 8
    %5 = sext i8 1 to i64
    %6 = sub i64 0, %5
    %7 = getelementptr inbounds i32, ptr %4, i64 %6
    store ptr %7, ptr %3, align 8
    %8 = load ptr, ptr %3, align 8
    %9 = load i32, ptr %8
    ret i32 %9
}

