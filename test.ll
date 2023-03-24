; ModuleID = 'test2.c'
source_filename = "test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@t = dso_local global double 1.000000e+00, align 8

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @test() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 @spread(i32 3)
  %3 = sitofp i32 %2 to double
  %4 = load double, double* @t, align 8
  %5 = fadd double %3, %4
  %6 = fptosi double %5 to i32
  store i32 %6, i32* %1, align 4
  %7 = load i32, i32* %1, align 4
  %8 = sitofp i32 %7 to double
  %9 = load double, double* @t, align 8
  %10 = fadd double %8, %9
  %11 = fsub double %10, 8.000000e+00
  %12 = fptosi double %11 to i32
  ret i32 %12
}

declare dso_local i32 @spread(i32) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32* @blah(i32* %0) #0 {
  %2 = alloca i32*, align 8
  %3 = alloca i32, align 4
  store i32* %0, i32** %2, align 8
  %4 = load i32*, i32** %2, align 8
  %5 = load i32, i32* %4, align 4
  %6 = call i32 @spread(i32 %5)
  store i32 %6, i32* %3, align 4
  store i32* %3, i32** %2, align 8
  %7 = load i32*, i32** %2, align 8
  ret i32* %7
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local float @test1() #0 {
  %1 = alloca float, align 4
  %2 = alloca i32, align 4
  store float 0x4023A56040000000, float* %1, align 4
  %3 = load float, float* %1, align 4
  %4 = fadd float %3, 8.000000e+00
  store float %4, float* %1, align 4
  %5 = load float, float* %1, align 4
  %6 = fpext float %5 to double
  %7 = fadd double %6, 2.900000e+00
  %8 = fptrunc double %7 to float
  store float %8, float* %1, align 4
  store i32 23, i32* %2, align 4
  %9 = load float, float* %1, align 4
  %10 = load i32, i32* %2, align 4
  %11 = sitofp i32 %10 to float
  %12 = fadd float %9, %11
  ret float %12
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
