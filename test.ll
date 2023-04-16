; ModuleID = 'test.S'
source_filename = "test.S"

@global = dso_local local_unnamed_addr global i32 42, align 4

; Function Attrs: norecurse nounwind readnone
define dso_local i32 @test1(i32 %0) local_unnamed_addr #0 {
  %2 = add i32 %0, 4
  ret i32 %2
}

; Function Attrs: norecurse nounwind readonly
define dso_local i32 @test() local_unnamed_addr #1 {
  %1 = load i32, i32* @global, align 4
  %2 = add i32 %1, 4
  ret i32 %2
}

attributes #0 = { norecurse nounwind readnone }
attributes #1 = { norecurse nounwind readonly }
