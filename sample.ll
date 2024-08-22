; ModuleID = 'sample.c'
source_filename = "sample.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.MyStruct = type { i32, i32 }

@a = dso_local global i32 0, align 4
@myArray = dso_local global [3 x i32] zeroinitializer, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @function1() #0 {
entry:
  %myStruct = alloca %struct.MyStruct, align 4
  store i32 10, ptr @a, align 4
  store i32 5, ptr @myArray, align 4
  %field0 = getelementptr inbounds nuw %struct.MyStruct, ptr %myStruct, i32 0, i32 0
  %0 = load i32, ptr %field0, align 4
  store i32 %0, ptr @myArray, align 4
  %1 = load i32, ptr @a, align 4
  %field1 = getelementptr inbounds nuw %struct.MyStruct, ptr %myStruct, i32 0, i32 1
  store i32 %1, ptr %field1, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @function2() #0 {
entry:
  %b = alloca i32, align 4
  store i32 20, ptr %b, align 4
  %0 = load i32, ptr %b, align 4
  store i32 %0, ptr @myArray, align 4
  %1 = load i32, ptr @a, align 4
  store i32 %1, ptr @myArray, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  call void @function1()
  call void @function2()
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 20.0.0git (https://github.com/llvm/llvm-project.git cba9166c0fca5f6c992c85a663019d58332d78e0)"}
