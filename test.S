@global = dso_local global i32 42, align 4

define dso_local i32 @test1(i32 %0) {
    %2 = alloca i32, align 4
    store i32 %0, i32* %2, align 4
    %3 = load i32, i32* %2, align 4
    %4 = add i32 %3, 4
    ret i32 %4
    ret i32 0
}

define dso_local i32 @test() {
    %1 = alloca i32, align 4
    store i32 4, i32* %1, align 4
    %2 = load i32, i32* %1, align 4
    %3 = load i32, i32* @global, align 4
    store i32 %3, i32* %1, align 4
    %4 = load i32, i32* %1, align 4
    %5 = load i32, i32* %1, align 4
    %6 = call i32 @test1(i32 %5)
    store i32 %6, i32* %1, align 4
    br label %7

7:
    %8 = load i32, i32* %1, align 4
    %9 = icmp ne i32 %8, 4
    br i1 %9, label %10, label %20

10:
    %11 = load i32, i32* %1, align 4
    %12 = sub i32 %11, 1
    store i32 %12, i32* %1, align 4
    %13 = load i32, i32* %1, align 4
    %14 = icmp ne i32 %13, 0
    br i1 %14, label %15, label %19

15:
    %16 = icmp eq i32 1, 0
    br i1 %16, label %17, label %18

17:
    br label %20

18:
    br label %19

19:
    br label %7

20:
    br label %21

21:
    %22 = load i32, i32* %1, align 4
    %23 = sub i32 %22, 1
    store i32 %23, i32* %1, align 4
    %24 = load i32, i32* %1, align 4
    %25 = icmp ne i32 %24, 0
    br i1 %25, label %26, label %30

26:
    %27 = icmp eq i32 1, 0
    br i1 %27, label %28, label %29

28:

    br label %29

29:
    br label %30

30:
    %31 = load i32, i32* %1, align 4
    %32 = icmp ne i32 %31, 4
    br label %33
33:
    %34 = load i32, i32* %1, align 4
    ret i32 %34
    ret i32 0
}

