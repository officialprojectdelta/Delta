define dso_local i32 @test() {
    %1 = icmp sgt i8 2, 0
    %2 = zext i1 %1 to i8
    %3 = icmp eq i8 2, %2
    %4 = zext i1 %3 to i32
    ret i32 %4
}

