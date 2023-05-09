define dso_local ptr @get_max(ptr %0, ptr %1) {
    %3 = alloca ptr, align 8
store ptr %0, ptr %3, align 8
    %4 = alloca ptr, align 8
store ptr %1, ptr %4, align 8
    %5 = load ptr, ptr %3, align 8
    %6 = load i32, ptr %5
    %7 = load ptr, ptr %4, align 8
    %8 = load i32, ptr %7
    %9 = icmp sgt i32 %6, %8
    br i1 %9, label %10, label %12

10:
    %11 = load ptr, ptr %3, align 8
    ret ptr %11

12:
    %13 = load ptr, ptr %4, align 8
    ret ptr %13

14:
    ret ptr null
}

define dso_local i32 @test() {
    %1 = alloca i32, align 4
store i32 5, ptr %1, align 4
    %2 = alloca i32, align 4
store i32 10, ptr %2, align 4
    %3 = alloca ptr, align 8
    %4 = load i32, ptr %1, align 4
    %5 = load i32, ptr %2, align 4
    %6 = call ptr @get_max(ptr %1, ptr %2)
store ptr %6, ptr %3, align 8
    %7 = load ptr, ptr %3, align 8
    %8 = load i32, ptr %7
    ret i32 %8
}

