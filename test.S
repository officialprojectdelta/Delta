define dso_local i32 @test() {
    %1 = icmp ne i8 1, 0
    br i1 %1, label %2, label %4

2:
    %3 = icmp ne i8 0, 0
    br label %5

4:
    br label %5

5:
    %6 = phi i1 [ %3, %2 ], [ 0, %4 ]
    %7 = zext i1 %6 to i32
    ret i32 %7
}

