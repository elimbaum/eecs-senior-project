; ModuleID = 'pythag.c'
source_filename = "pythag.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@.str = private unnamed_addr constant [18 x i8] c"(%f) %d %d -> %d\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32, i8**) #0 !dbg !10 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca double, align 8
  %9 = alloca double, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !16, metadata !DIExpression()), !dbg !17
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !18, metadata !DIExpression()), !dbg !19
  call void @llvm.dbg.declare(metadata i32* %6, metadata !20, metadata !DIExpression()), !dbg !22
  store i32 1, i32* %6, align 4, !dbg !22
  br label %10, !dbg !23

; <label>:10:                                     ; preds = %51, %2
  %11 = load i32, i32* %6, align 4, !dbg !24
  %12 = icmp sle i32 %11, 50, !dbg !26
  br i1 %12, label %13, label %54, !dbg !27

; <label>:13:                                     ; preds = %10
  call void @llvm.dbg.declare(metadata i32* %7, metadata !28, metadata !DIExpression()), !dbg !31
  store i32 1, i32* %7, align 4, !dbg !31
  br label %14, !dbg !32

; <label>:14:                                     ; preds = %47, %13
  %15 = load i32, i32* %7, align 4, !dbg !33
  %16 = load i32, i32* %6, align 4, !dbg !35
  %17 = sdiv i32 %16, 2, !dbg !36
  %18 = icmp sle i32 %15, %17, !dbg !37
  br i1 %18, label %19, label %50, !dbg !38

; <label>:19:                                     ; preds = %14
  call void @llvm.dbg.declare(metadata double* %8, metadata !39, metadata !DIExpression()), !dbg !41
  %20 = load i32, i32* %6, align 4, !dbg !42
  %21 = load i32, i32* %7, align 4, !dbg !43
  %22 = sub nsw i32 %20, %21, !dbg !44
  %23 = sitofp i32 %22 to double, !dbg !42
  %24 = load i32, i32* %7, align 4, !dbg !45
  %25 = sitofp i32 %24 to double, !dbg !45
  %26 = call double @hypot(double %23, double %25) #4, !dbg !46
  store double %26, double* %8, align 8, !dbg !41
  call void @llvm.dbg.declare(metadata double* %9, metadata !47, metadata !DIExpression()), !dbg !48
  %27 = load double, double* %8, align 8, !dbg !49
  %28 = call double @fmod(double %27, double 1.000000e+00) #4, !dbg !50
  store double %28, double* %9, align 8, !dbg !48
  %29 = load double, double* %9, align 8, !dbg !51
  %30 = fcmp oeq double %29, 0.000000e+00, !dbg !53
  br i1 %30, label %31, label %46, !dbg !54

; <label>:31:                                     ; preds = %19
  %32 = load i32, i32* %7, align 4, !dbg !55
  %33 = sitofp i32 %32 to double, !dbg !57
  %34 = load i32, i32* %6, align 4, !dbg !58
  %35 = load i32, i32* %7, align 4, !dbg !59
  %36 = sub nsw i32 %34, %35, !dbg !60
  %37 = sitofp i32 %36 to double, !dbg !61
  %38 = fdiv double %33, %37, !dbg !62
  %39 = load i32, i32* %7, align 4, !dbg !63
  %40 = load i32, i32* %6, align 4, !dbg !64
  %41 = load i32, i32* %7, align 4, !dbg !65
  %42 = sub nsw i32 %40, %41, !dbg !66
  %43 = load double, double* %8, align 8, !dbg !67
  %44 = fptosi double %43 to i32, !dbg !68
  %45 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([18 x i8], [18 x i8]* @.str, i64 0, i64 0), double %38, i32 %39, i32 %42, i32 %44), !dbg !69
  br label %46, !dbg !70

; <label>:46:                                     ; preds = %31, %19
  br label %47, !dbg !71

; <label>:47:                                     ; preds = %46
  %48 = load i32, i32* %7, align 4, !dbg !72
  %49 = add nsw i32 %48, 1, !dbg !72
  store i32 %49, i32* %7, align 4, !dbg !72
  br label %14, !dbg !73, !llvm.loop !74

; <label>:50:                                     ; preds = %14
  br label %51, !dbg !76

; <label>:51:                                     ; preds = %50
  %52 = load i32, i32* %6, align 4, !dbg !77
  %53 = add nsw i32 %52, 1, !dbg !77
  store i32 %53, i32* %6, align 4, !dbg !77
  br label %10, !dbg !78, !llvm.loop !79

; <label>:54:                                     ; preds = %10
  %55 = load i32, i32* %3, align 4, !dbg !81
  ret i32 %55, !dbg !81
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind
declare dso_local double @hypot(double, double) #2

; Function Attrs: nounwind
declare dso_local double @fmod(double, double) #2

declare dso_local i32 @printf(i8*, ...) #3

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 9.0.0 (https://github.com/llvm/llvm-project.git d0156256e2c22f8d7768400375ae6e3d3f1bf5e1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2, retainedTypes: !3, nameTableKind: None)
!1 = !DIFile(filename: "pythag.c", directory: "/home/eli/sproj-git/blas")
!2 = !{}
!3 = !{!4, !5}
!4 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!5 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!6 = !{i32 2, !"Dwarf Version", i32 4}
!7 = !{i32 2, !"Debug Info Version", i32 3}
!8 = !{i32 1, !"wchar_size", i32 4}
!9 = !{!"clang version 9.0.0 (https://github.com/llvm/llvm-project.git d0156256e2c22f8d7768400375ae6e3d3f1bf5e1)"}
!10 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 11, type: !11, scopeLine: 11, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !2)
!11 = !DISubroutineType(types: !12)
!12 = !{!5, !5, !13}
!13 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !14, size: 64)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64)
!15 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!16 = !DILocalVariable(name: "argc", arg: 1, scope: !10, file: !1, line: 11, type: !5)
!17 = !DILocation(line: 11, column: 14, scope: !10)
!18 = !DILocalVariable(name: "argv", arg: 2, scope: !10, file: !1, line: 11, type: !13)
!19 = !DILocation(line: 11, column: 28, scope: !10)
!20 = !DILocalVariable(name: "x", scope: !21, file: !1, line: 13, type: !5)
!21 = distinct !DILexicalBlock(scope: !10, file: !1, line: 13, column: 3)
!22 = !DILocation(line: 13, column: 12, scope: !21)
!23 = !DILocation(line: 13, column: 8, scope: !21)
!24 = !DILocation(line: 13, column: 19, scope: !25)
!25 = distinct !DILexicalBlock(scope: !21, file: !1, line: 13, column: 3)
!26 = !DILocation(line: 13, column: 21, scope: !25)
!27 = !DILocation(line: 13, column: 3, scope: !21)
!28 = !DILocalVariable(name: "y", scope: !29, file: !1, line: 15, type: !5)
!29 = distinct !DILexicalBlock(scope: !30, file: !1, line: 15, column: 5)
!30 = distinct !DILexicalBlock(scope: !25, file: !1, line: 13, column: 36)
!31 = !DILocation(line: 15, column: 13, scope: !29)
!32 = !DILocation(line: 15, column: 9, scope: !29)
!33 = !DILocation(line: 15, column: 20, scope: !34)
!34 = distinct !DILexicalBlock(scope: !29, file: !1, line: 15, column: 5)
!35 = !DILocation(line: 15, column: 25, scope: !34)
!36 = !DILocation(line: 15, column: 27, scope: !34)
!37 = !DILocation(line: 15, column: 22, scope: !34)
!38 = !DILocation(line: 15, column: 5, scope: !29)
!39 = !DILocalVariable(name: "c", scope: !40, file: !1, line: 16, type: !4)
!40 = distinct !DILexicalBlock(scope: !34, file: !1, line: 15, column: 37)
!41 = !DILocation(line: 16, column: 14, scope: !40)
!42 = !DILocation(line: 16, column: 24, scope: !40)
!43 = !DILocation(line: 16, column: 28, scope: !40)
!44 = !DILocation(line: 16, column: 26, scope: !40)
!45 = !DILocation(line: 16, column: 31, scope: !40)
!46 = !DILocation(line: 16, column: 18, scope: !40)
!47 = !DILocalVariable(name: "r", scope: !40, file: !1, line: 17, type: !4)
!48 = !DILocation(line: 17, column: 14, scope: !40)
!49 = !DILocation(line: 17, column: 23, scope: !40)
!50 = !DILocation(line: 17, column: 18, scope: !40)
!51 = !DILocation(line: 18, column: 11, scope: !52)
!52 = distinct !DILexicalBlock(scope: !40, file: !1, line: 18, column: 11)
!53 = !DILocation(line: 18, column: 13, scope: !52)
!54 = !DILocation(line: 18, column: 11, scope: !40)
!55 = !DILocation(line: 19, column: 47, scope: !56)
!56 = distinct !DILexicalBlock(scope: !52, file: !1, line: 18, column: 19)
!57 = !DILocation(line: 19, column: 38, scope: !56)
!58 = !DILocation(line: 19, column: 52, scope: !56)
!59 = !DILocation(line: 19, column: 56, scope: !56)
!60 = !DILocation(line: 19, column: 54, scope: !56)
!61 = !DILocation(line: 19, column: 51, scope: !56)
!62 = !DILocation(line: 19, column: 49, scope: !56)
!63 = !DILocation(line: 19, column: 60, scope: !56)
!64 = !DILocation(line: 19, column: 63, scope: !56)
!65 = !DILocation(line: 19, column: 67, scope: !56)
!66 = !DILocation(line: 19, column: 65, scope: !56)
!67 = !DILocation(line: 19, column: 75, scope: !56)
!68 = !DILocation(line: 19, column: 70, scope: !56)
!69 = !DILocation(line: 19, column: 9, scope: !56)
!70 = !DILocation(line: 20, column: 7, scope: !56)
!71 = !DILocation(line: 21, column: 5, scope: !40)
!72 = !DILocation(line: 15, column: 33, scope: !34)
!73 = !DILocation(line: 15, column: 5, scope: !34)
!74 = distinct !{!74, !38, !75}
!75 = !DILocation(line: 21, column: 5, scope: !29)
!76 = !DILocation(line: 22, column: 3, scope: !30)
!77 = !DILocation(line: 13, column: 32, scope: !25)
!78 = !DILocation(line: 13, column: 3, scope: !25)
!79 = distinct !{!79, !27, !80}
!80 = !DILocation(line: 22, column: 3, scope: !21)
!81 = !DILocation(line: 23, column: 1, scope: !10)
