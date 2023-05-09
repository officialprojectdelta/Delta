; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@t = dso_local global double 1.000000e+00, align 8

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @test() #0 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  store i32 3, ptr %1, align 4
  store ptr %1, ptr %2, align 8
  %4 = load ptr, ptr %2, align 8
  store i32 3, ptr %4, align 4
  %5 = call i32 @spread(i32 noundef 3)
  %6 = sitofp i32 %5 to double
  %7 = load double, ptr @t, align 8
  %8 = fadd double %6, %7
  %9 = call float @test1()
  %10 = fpext float %9 to double
  %11 = fadd double %8, %10
  %12 = fptosi double %11 to i32
  store i32 %12, ptr %3, align 4
  %13 = load i32, ptr %3, align 4
  %14 = sitofp i32 %13 to double
  %15 = load double, ptr @t, align 8
  %16 = fadd double %14, %15
  %17 = fsub double %16, 8.000000e+00
  %18 = fptosi double %17 to i32
  ret i32 %18
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local i32 @spread(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i16, align 2
  store i32 %0, ptr %2, align 4
  store i32 0, ptr %3, align 4
  br label %8

8:                                                ; preds = %16, %1
  %9 = load i32, ptr %3, align 4
  %10 = icmp slt i32 %9, 4
  br i1 %10, label %11, label %19

11:                                               ; preds = %8
  %12 = load i32, ptr %2, align 4
  %13 = load i32, ptr %3, align 4
  %14 = sdiv i32 %13, 4
  %15 = add nsw i32 %12, %14
  store i32 %15, ptr %2, align 4
  br label %16

16:                                               ; preds = %11
  %17 = load i32, ptr %3, align 4
  %18 = add nsw i32 %17, 1
  store i32 %18, ptr %3, align 4
  br label %8, !llvm.loop !6

19:                                               ; preds = %8
  store float 0x4002666660000000, ptr %4, align 4
  %20 = load float, ptr %4, align 4
  %21 = fcmp une float %20, 0.000000e+00
  %22 = xor i1 %21, true
  %23 = zext i1 %22 to i32
  store i32 %23, ptr %5, align 4
  %24 = load float, ptr %4, align 4
  %25 = fneg float %24
  %26 = fptosi float %25 to i32
  store i32 %26, ptr %5, align 4
  %27 = load i32, ptr %5, align 4
  %28 = add nsw i32 %27, 1
  store i32 %28, ptr %5, align 4
  %29 = add nsw i32 %27, 3
  store i32 %29, ptr %6, align 4
  %30 = load i32, ptr %6, align 4
  %31 = trunc i32 %30 to i16
  store i16 %31, ptr %7, align 2
  %32 = load i32, ptr %2, align 4
  ret i32 %32
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local ptr @blah(ptr noundef %0) #0 {
  %2 = alloca ptr, align 8
  %3 = alloca ptr, align 8
  %4 = alloca i32, align 4
  store ptr %0, ptr %2, align 8
  store ptr %2, ptr %3, align 8
  %5 = load ptr, ptr %2, align 8
  %6 = load i32, ptr %5, align 4
  %7 = call i32 @spread(i32 noundef %6)
  store i32 %7, ptr %4, align 4
  store ptr %4, ptr %2, align 8
  %8 = load ptr, ptr %2, align 8
  ret ptr %8
}

; Function Attrs: noinline nounwind optnone sspstrong uwtable
define dso_local float @test1() #0 {
  %1 = alloca float, align 4
  %2 = alloca i32, align 4
  store float 0x4023A56040000000, ptr %1, align 4
  %3 = load float, ptr %1, align 4
  %4 = fadd float %3, 8.000000e+00
  store float %4, ptr %1, align 4
  %5 = load float, ptr %1, align 4
  %6 = fpext float %5 to double
  %7 = fadd double %6, 2.900000e+00
  %8 = fptrunc double %7 to float
  store float %8, ptr %1, align 4
  store i32 23, ptr %2, align 4
  store double 9.000000e+00, ptr @t, align 8
  %9 = load float, ptr %1, align 4
  %10 = load i32, ptr %2, align 4
  %11 = sitofp i32 %10 to float
  %12 = fadd float %9, %11
  ret float %12
}

attributes #0 = { noinline nounwind optnone sspstrong uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 15.0.7"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
