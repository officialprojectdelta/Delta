; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@global = dso_local global i32 42, align 4
@gb2 = dso_local global i8 -115, align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local signext i8 @test1(i32 %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i8, align 1
  store i32 %0, i32* %2, align 4
  %4 = load i32, i32* %2, align 4
  %5 = trunc i32 %4 to i8
  store i8 %5, i8* %3, align 1
  %6 = load i8, i8* %3, align 1
  ret i8 %6
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @test() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i16, align 2
  %3 = alloca i32, align 4
  %4 = alloca float, align 4
  store i32 -5, i32* %1, align 4
  %5 = load i32, i32* %1, align 4
  %6 = trunc i32 %5 to i16
  store i16 %6, i16* %2, align 2
  store i32 4, i32* %3, align 4
  %7 = load i32, i32* @global, align 4
  store i32 %7, i32* %3, align 4
  store float 0x3F53A92A40000000, float* %4, align 4
  %8 = load float, float* %4, align 4
  %9 = fptosi float %8 to i32
  %10 = call signext i8 @test1(i32 %9)
  %11 = sext i8 %10 to i32
  ret i32 %11
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
