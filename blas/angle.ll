; ModuleID = 'angle.c'
source_filename = "angle.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct._IO_FILE = type { i32, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, i8*, %struct._IO_marker*, %struct._IO_FILE*, i32, i32, i64, i16, i8, [1 x i8], i8*, i64, i8*, i8*, i8*, i8*, i64, i32, [20 x i8] }
%struct._IO_marker = type { %struct._IO_marker*, %struct._IO_FILE*, i32 }

@state = dso_local global i32 0, align 4, !dbg !0
@stderr = external dso_local global %struct._IO_FILE*, align 8
@.str = private unnamed_addr constant [26 x i8] c"Usage: %s [input answer]\0A\00", align 1
@.str.1 = private unnamed_addr constant [9 x i8] c"angle.in\00", align 1
@.str.2 = private unnamed_addr constant [10 x i8] c"angle.ans\00", align 1
@.str.3 = private unnamed_addr constant [2 x i8] c"r\00", align 1
@.str.4 = private unnamed_addr constant [42 x i8] c"%s: length mismatch on line %d: %d vs %d\0A\00", align 1
@.str.5 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@.str.6 = private unnamed_addr constant [42 x i8] c"%s: expected ';' on line %d, got %c (%u)\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main(i32, i8**) #0 !dbg !16 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  %6 = alloca i8*, align 8
  %7 = alloca i8*, align 8
  %8 = alloca %struct._IO_FILE*, align 8
  %9 = alloca %struct._IO_FILE*, align 8
  %10 = alloca i32, align 4
  %11 = alloca i8*, align 8
  %12 = alloca i8*, align 8
  %13 = alloca i64, align 8
  %14 = alloca i8*, align 8
  %15 = alloca i32, align 4
  %16 = alloca i32, align 4
  %17 = alloca double*, align 8
  %18 = alloca double*, align 8
  %19 = alloca double*, align 8
  %20 = alloca i32, align 4
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  call void @llvm.dbg.declare(metadata i32* %4, metadata !24, metadata !DIExpression()), !dbg !25
  store i8** %1, i8*** %5, align 8
  call void @llvm.dbg.declare(metadata i8*** %5, metadata !26, metadata !DIExpression()), !dbg !27
  %21 = load i32, i32* %4, align 4, !dbg !28
  %22 = icmp eq i32 %21, 2, !dbg !30
  br i1 %22, label %26, label %23, !dbg !31

; <label>:23:                                     ; preds = %2
  %24 = load i32, i32* %4, align 4, !dbg !32
  %25 = icmp sgt i32 %24, 3, !dbg !33
  br i1 %25, label %26, label %32, !dbg !34

; <label>:26:                                     ; preds = %23, %2
  %27 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !35
  %28 = load i8**, i8*** %5, align 8, !dbg !35
  %29 = getelementptr inbounds i8*, i8** %28, i64 0, !dbg !35
  %30 = load i8*, i8** %29, align 8, !dbg !35
  %31 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %27, i8* getelementptr inbounds ([26 x i8], [26 x i8]* @.str, i64 0, i64 0), i8* %30), !dbg !35
  call void @exit(i32 1) #5, !dbg !35
  unreachable, !dbg !35

; <label>:32:                                     ; preds = %23
  call void @llvm.dbg.declare(metadata i8** %6, metadata !37, metadata !DIExpression()), !dbg !38
  store i8* getelementptr inbounds ([9 x i8], [9 x i8]* @.str.1, i64 0, i64 0), i8** %6, align 8, !dbg !38
  call void @llvm.dbg.declare(metadata i8** %7, metadata !39, metadata !DIExpression()), !dbg !40
  store i8* getelementptr inbounds ([10 x i8], [10 x i8]* @.str.2, i64 0, i64 0), i8** %7, align 8, !dbg !40
  %33 = load i32, i32* %4, align 4, !dbg !41
  %34 = icmp eq i32 %33, 3, !dbg !43
  br i1 %34, label %35, label %42, !dbg !44

; <label>:35:                                     ; preds = %32
  %36 = load i8**, i8*** %5, align 8, !dbg !45
  %37 = getelementptr inbounds i8*, i8** %36, i64 1, !dbg !45
  %38 = load i8*, i8** %37, align 8, !dbg !45
  store i8* %38, i8** %6, align 8, !dbg !47
  %39 = load i8**, i8*** %5, align 8, !dbg !48
  %40 = getelementptr inbounds i8*, i8** %39, i64 2, !dbg !48
  %41 = load i8*, i8** %40, align 8, !dbg !48
  store i8* %41, i8** %7, align 8, !dbg !49
  br label %42, !dbg !50

; <label>:42:                                     ; preds = %35, %32
  call void @llvm.dbg.declare(metadata %struct._IO_FILE** %8, metadata !51, metadata !DIExpression()), !dbg !112
  %43 = load i8*, i8** %6, align 8, !dbg !113
  %44 = call %struct._IO_FILE* @fopen(i8* %43, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.3, i64 0, i64 0)), !dbg !114
  store %struct._IO_FILE* %44, %struct._IO_FILE** %8, align 8, !dbg !112
  %45 = load %struct._IO_FILE*, %struct._IO_FILE** %8, align 8, !dbg !115
  %46 = icmp ne %struct._IO_FILE* %45, null, !dbg !115
  br i1 %46, label %49, label %47, !dbg !117

; <label>:47:                                     ; preds = %42
  %48 = load i8*, i8** %6, align 8, !dbg !118
  call void @perror(i8* %48), !dbg !118
  call void @exit(i32 1) #5, !dbg !118
  unreachable, !dbg !118

; <label>:49:                                     ; preds = %42
  call void @llvm.dbg.declare(metadata %struct._IO_FILE** %9, metadata !119, metadata !DIExpression()), !dbg !120
  %50 = load i8*, i8** %7, align 8, !dbg !121
  %51 = call %struct._IO_FILE* @fopen(i8* %50, i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.3, i64 0, i64 0)), !dbg !122
  store %struct._IO_FILE* %51, %struct._IO_FILE** %9, align 8, !dbg !120
  %52 = load %struct._IO_FILE*, %struct._IO_FILE** %9, align 8, !dbg !123
  %53 = icmp ne %struct._IO_FILE* %52, null, !dbg !123
  br i1 %53, label %56, label %54, !dbg !125

; <label>:54:                                     ; preds = %49
  %55 = load i8*, i8** %7, align 8, !dbg !126
  call void @perror(i8* %55), !dbg !126
  call void @exit(i32 1) #5, !dbg !126
  unreachable, !dbg !126

; <label>:56:                                     ; preds = %49
  call void @llvm.dbg.declare(metadata i32* %10, metadata !127, metadata !DIExpression()), !dbg !128
  store i32 0, i32* %10, align 4, !dbg !128
  call void @llvm.dbg.declare(metadata i8** %11, metadata !129, metadata !DIExpression()), !dbg !130
  store i8* null, i8** %11, align 8, !dbg !130
  call void @llvm.dbg.declare(metadata i8** %12, metadata !131, metadata !DIExpression()), !dbg !132
  call void @llvm.dbg.declare(metadata i64* %13, metadata !133, metadata !DIExpression()), !dbg !134
  call void @llvm.dbg.declare(metadata i8** %14, metadata !135, metadata !DIExpression()), !dbg !136
  call void @llvm.dbg.declare(metadata i32* %15, metadata !137, metadata !DIExpression()), !dbg !138
  store i32 0, i32* %15, align 4, !dbg !138
  call void @llvm.dbg.declare(metadata i32* %16, metadata !139, metadata !DIExpression()), !dbg !140
  store i32 0, i32* %16, align 4, !dbg !140
  call void @llvm.dbg.declare(metadata double** %17, metadata !141, metadata !DIExpression()), !dbg !144
  call void @llvm.dbg.declare(metadata double** %18, metadata !145, metadata !DIExpression()), !dbg !146
  %57 = call noalias i8* @malloc(i64 128) #6, !dbg !147
  %58 = bitcast i8* %57 to double*, !dbg !147
  store double* %58, double** %18, align 8, !dbg !146
  call void @llvm.dbg.declare(metadata double** %19, metadata !148, metadata !DIExpression()), !dbg !149
  %59 = call noalias i8* @malloc(i64 128) #6, !dbg !150
  %60 = bitcast i8* %59 to double*, !dbg !150
  store double* %60, double** %19, align 8, !dbg !149
  call void @llvm.dbg.declare(metadata i32* %20, metadata !151, metadata !DIExpression()), !dbg !152
  store i32 16, i32* %20, align 4, !dbg !152
  br label %61, !dbg !153

; <label>:61:                                     ; preds = %182, %96, %56
  %62 = load %struct._IO_FILE*, %struct._IO_FILE** %8, align 8, !dbg !154
  %63 = call i64 @getline(i8** %11, i64* %13, %struct._IO_FILE* %62), !dbg !155
  %64 = icmp sgt i64 %63, 0, !dbg !156
  br i1 %64, label %65, label %183, !dbg !153

; <label>:65:                                     ; preds = %61
  %66 = load i32, i32* %10, align 4, !dbg !157
  %67 = add nsw i32 %66, 1, !dbg !157
  store i32 %67, i32* %10, align 4, !dbg !157
  %68 = load i8*, i8** %11, align 8, !dbg !159
  store i8* %68, i8** %12, align 8, !dbg !160
  %69 = load i32, i32* @state, align 4, !dbg !161
  %70 = icmp eq i32 %69, 0, !dbg !163
  br i1 %70, label %71, label %73, !dbg !164

; <label>:71:                                     ; preds = %65
  %72 = load double*, double** %18, align 8, !dbg !165
  store double* %72, double** %17, align 8, !dbg !167
  store i32 1, i32* @state, align 4, !dbg !168
  br label %125, !dbg !169

; <label>:73:                                     ; preds = %65
  %74 = load i32, i32* @state, align 4, !dbg !170
  %75 = icmp eq i32 %74, 1, !dbg !172
  br i1 %75, label %76, label %79, !dbg !173

; <label>:76:                                     ; preds = %73
  %77 = load i32, i32* %16, align 4, !dbg !174
  store i32 %77, i32* %15, align 4, !dbg !176
  %78 = load double*, double** %19, align 8, !dbg !177
  store double* %78, double** %17, align 8, !dbg !178
  store i32 2, i32* @state, align 4, !dbg !179
  br label %124, !dbg !180

; <label>:79:                                     ; preds = %73
  %80 = load i8*, i8** %12, align 8, !dbg !181
  %81 = getelementptr inbounds i8, i8* %80, i64 0, !dbg !181
  %82 = load i8, i8* %81, align 1, !dbg !181
  %83 = sext i8 %82 to i32, !dbg !181
  %84 = icmp eq i32 %83, 59, !dbg !184
  br i1 %84, label %85, label %111, !dbg !185

; <label>:85:                                     ; preds = %79
  %86 = load i32, i32* %16, align 4, !dbg !186
  %87 = load i32, i32* %15, align 4, !dbg !189
  %88 = icmp ne i32 %86, %87, !dbg !190
  br i1 %88, label %89, label %96, !dbg !191

; <label>:89:                                     ; preds = %85
  %90 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !192
  %91 = load i8*, i8** %6, align 8, !dbg !192
  %92 = load i32, i32* %10, align 4, !dbg !192
  %93 = load i32, i32* %15, align 4, !dbg !192
  %94 = load i32, i32* %16, align 4, !dbg !192
  %95 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %90, i8* getelementptr inbounds ([42 x i8], [42 x i8]* @.str.4, i64 0, i64 0), i8* %91, i32 %92, i32 %93, i32 %94), !dbg !192
  call void @exit(i32 1) #5, !dbg !192
  unreachable, !dbg !192

; <label>:96:                                     ; preds = %85
  %97 = load i32, i32* %16, align 4, !dbg !194
  %98 = load double*, double** %18, align 8, !dbg !195
  %99 = load double*, double** %19, align 8, !dbg !196
  %100 = call double @cblas_ddot(i32 %97, double* %98, i32 1, double* %99, i32 1), !dbg !197
  %101 = load i32, i32* %16, align 4, !dbg !198
  %102 = load double*, double** %18, align 8, !dbg !199
  %103 = call double @cblas_dnrm2(i32 %101, double* %102, i32 1), !dbg !200
  %104 = load i32, i32* %16, align 4, !dbg !201
  %105 = load double*, double** %19, align 8, !dbg !202
  %106 = call double @cblas_dnrm2(i32 %104, double* %105, i32 1), !dbg !203
  %107 = fmul double %103, %106, !dbg !204
  %108 = fdiv double %100, %107, !dbg !205
  %109 = call double @acos(double %108) #6, !dbg !206
  %110 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.5, i64 0, i64 0), double %109), !dbg !207
  store i32 0, i32* @state, align 4, !dbg !208
  br label %61, !dbg !209, !llvm.loop !210

; <label>:111:                                    ; preds = %79
  %112 = load %struct._IO_FILE*, %struct._IO_FILE** @stderr, align 8, !dbg !212
  %113 = load i8*, i8** %6, align 8, !dbg !212
  %114 = load i32, i32* %10, align 4, !dbg !212
  %115 = load i8*, i8** %12, align 8, !dbg !212
  %116 = getelementptr inbounds i8, i8* %115, i64 0, !dbg !212
  %117 = load i8, i8* %116, align 1, !dbg !212
  %118 = sext i8 %117 to i32, !dbg !212
  %119 = load i8*, i8** %12, align 8, !dbg !212
  %120 = getelementptr inbounds i8, i8* %119, i64 0, !dbg !212
  %121 = load i8, i8* %120, align 1, !dbg !212
  %122 = sext i8 %121 to i32, !dbg !212
  %123 = call i32 (%struct._IO_FILE*, i8*, ...) @fprintf(%struct._IO_FILE* %112, i8* getelementptr inbounds ([42 x i8], [42 x i8]* @.str.6, i64 0, i64 0), i8* %113, i32 %114, i32 %118, i32 %122), !dbg !212
  call void @exit(i32 1) #5, !dbg !212
  unreachable, !dbg !212

; <label>:124:                                    ; preds = %76
  br label %125

; <label>:125:                                    ; preds = %124, %71
  store i32 0, i32* %16, align 4, !dbg !214
  br label %126, !dbg !215

; <label>:126:                                    ; preds = %181, %125
  %127 = load i8*, i8** %12, align 8, !dbg !216
  %128 = load i8, i8* %127, align 1, !dbg !217
  %129 = sext i8 %128 to i32, !dbg !217
  %130 = icmp ne i32 %129, 10, !dbg !218
  br i1 %130, label %131, label %182, !dbg !215

; <label>:131:                                    ; preds = %126
  %132 = load i8*, i8** %12, align 8, !dbg !219
  %133 = call double @strtod(i8* %132, i8** %14) #6, !dbg !221
  %134 = load double*, double** %17, align 8, !dbg !222
  %135 = load i32, i32* %16, align 4, !dbg !223
  %136 = sext i32 %135 to i64, !dbg !222
  %137 = getelementptr inbounds double, double* %134, i64 %136, !dbg !222
  store double %133, double* %137, align 8, !dbg !224
  %138 = load i8*, i8** %14, align 8, !dbg !225
  %139 = load i8*, i8** %12, align 8, !dbg !227
  %140 = icmp eq i8* %138, %139, !dbg !228
  br i1 %140, label %141, label %142, !dbg !229

; <label>:141:                                    ; preds = %131
  br label %182, !dbg !230

; <label>:142:                                    ; preds = %131
  %143 = load i32, i32* %16, align 4, !dbg !231
  %144 = add nsw i32 %143, 1, !dbg !231
  store i32 %144, i32* %16, align 4, !dbg !231
  %145 = load i32, i32* %20, align 4, !dbg !233
  %146 = icmp sge i32 %144, %145, !dbg !234
  br i1 %146, label %147, label %172, !dbg !235

; <label>:147:                                    ; preds = %142
  %148 = load i32, i32* %20, align 4, !dbg !236
  %149 = mul nsw i32 %148, 2, !dbg !236
  store i32 %149, i32* %20, align 4, !dbg !236
  %150 = load double*, double** %18, align 8, !dbg !238
  %151 = bitcast double* %150 to i8*, !dbg !238
  %152 = load i32, i32* %20, align 4, !dbg !239
  %153 = sext i32 %152 to i64, !dbg !239
  %154 = mul i64 %153, 8, !dbg !240
  %155 = call i8* @realloc(i8* %151, i64 %154) #6, !dbg !241
  %156 = bitcast i8* %155 to double*, !dbg !241
  store double* %156, double** %18, align 8, !dbg !242
  %157 = load double*, double** %19, align 8, !dbg !243
  %158 = bitcast double* %157 to i8*, !dbg !243
  %159 = load i32, i32* %20, align 4, !dbg !244
  %160 = sext i32 %159 to i64, !dbg !244
  %161 = mul i64 %160, 8, !dbg !245
  %162 = call i8* @realloc(i8* %158, i64 %161) #6, !dbg !246
  %163 = bitcast i8* %162 to double*, !dbg !246
  store double* %163, double** %19, align 8, !dbg !247
  %164 = load i32, i32* @state, align 4, !dbg !248
  %165 = icmp eq i32 %164, 1, !dbg !249
  br i1 %165, label %166, label %168, !dbg !250

; <label>:166:                                    ; preds = %147
  %167 = load double*, double** %18, align 8, !dbg !251
  br label %170, !dbg !250

; <label>:168:                                    ; preds = %147
  %169 = load double*, double** %19, align 8, !dbg !252
  br label %170, !dbg !250

; <label>:170:                                    ; preds = %168, %166
  %171 = phi double* [ %167, %166 ], [ %169, %168 ], !dbg !250
  store double* %171, double** %17, align 8, !dbg !253
  br label %172, !dbg !254

; <label>:172:                                    ; preds = %170, %142
  %173 = load i8*, i8** %14, align 8, !dbg !255
  store i8* %173, i8** %12, align 8, !dbg !256
  br label %174, !dbg !257

; <label>:174:                                    ; preds = %180, %172
  %175 = load i8*, i8** %12, align 8, !dbg !258
  %176 = getelementptr inbounds i8, i8* %175, i32 1, !dbg !258
  store i8* %176, i8** %12, align 8, !dbg !258
  %177 = load i8, i8* %176, align 1, !dbg !259
  %178 = sext i8 %177 to i32, !dbg !259
  %179 = icmp eq i32 %178, 32, !dbg !260
  br i1 %179, label %180, label %181, !dbg !257

; <label>:180:                                    ; preds = %174
  br label %174, !dbg !257, !llvm.loop !261

; <label>:181:                                    ; preds = %174
  br label %126, !dbg !215, !llvm.loop !263

; <label>:182:                                    ; preds = %141, %126
  br label %61, !dbg !153, !llvm.loop !210

; <label>:183:                                    ; preds = %61
  %184 = load i32, i32* %3, align 4, !dbg !265
  ret i32 %184, !dbg !265
}

; Function Attrs: nounwind readnone speculatable
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare dso_local i32 @fprintf(%struct._IO_FILE*, i8*, ...) #2

; Function Attrs: noreturn nounwind
declare dso_local void @exit(i32) #3

declare dso_local %struct._IO_FILE* @fopen(i8*, i8*) #2

declare dso_local void @perror(i8*) #2

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #4

declare dso_local i64 @getline(i8**, i64*, %struct._IO_FILE*) #2

declare dso_local i32 @printf(i8*, ...) #2

; Function Attrs: nounwind
declare dso_local double @acos(double) #4

declare dso_local double @cblas_ddot(i32, double*, i32, double*, i32) #2

declare dso_local double @cblas_dnrm2(i32, double*, i32) #2

; Function Attrs: nounwind
declare dso_local double @strtod(i8*, i8**) #4

; Function Attrs: nounwind
declare dso_local i8* @realloc(i8*, i64) #4

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone speculatable }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { noreturn nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noreturn nounwind }
attributes #6 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!12, !13, !14}
!llvm.ident = !{!15}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "state", scope: !2, file: !3, line: 15, type: !5, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "clang version 9.0.0 (https://github.com/llvm/llvm-project.git d0156256e2c22f8d7768400375ae6e3d3f1bf5e1)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !11, nameTableKind: None)
!3 = !DIFile(filename: "angle.c", directory: "/home/eli/sproj-git/blas")
!4 = !{!5}
!5 = !DICompositeType(tag: DW_TAG_enumeration_type, name: "State", file: !3, line: 15, baseType: !6, size: 32, elements: !7)
!6 = !DIBasicType(name: "unsigned int", size: 32, encoding: DW_ATE_unsigned)
!7 = !{!8, !9, !10}
!8 = !DIEnumerator(name: "ST_A", value: 0, isUnsigned: true)
!9 = !DIEnumerator(name: "ST_B", value: 1, isUnsigned: true)
!10 = !DIEnumerator(name: "ST_SEP", value: 2, isUnsigned: true)
!11 = !{!0}
!12 = !{i32 2, !"Dwarf Version", i32 4}
!13 = !{i32 2, !"Debug Info Version", i32 3}
!14 = !{i32 1, !"wchar_size", i32 4}
!15 = !{!"clang version 9.0.0 (https://github.com/llvm/llvm-project.git d0156256e2c22f8d7768400375ae6e3d3f1bf5e1)"}
!16 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 17, type: !17, scopeLine: 17, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !23)
!17 = !DISubroutineType(types: !18)
!18 = !{!19, !19, !20}
!19 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!20 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !21, size: 64)
!21 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !22, size: 64)
!22 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!23 = !{}
!24 = !DILocalVariable(name: "argc", arg: 1, scope: !16, file: !3, line: 17, type: !19)
!25 = !DILocation(line: 17, column: 14, scope: !16)
!26 = !DILocalVariable(name: "argv", arg: 2, scope: !16, file: !3, line: 17, type: !20)
!27 = !DILocation(line: 17, column: 28, scope: !16)
!28 = !DILocation(line: 19, column: 7, scope: !29)
!29 = distinct !DILexicalBlock(scope: !16, file: !3, line: 19, column: 7)
!30 = !DILocation(line: 19, column: 12, scope: !29)
!31 = !DILocation(line: 19, column: 17, scope: !29)
!32 = !DILocation(line: 19, column: 20, scope: !29)
!33 = !DILocation(line: 19, column: 25, scope: !29)
!34 = !DILocation(line: 19, column: 7, scope: !16)
!35 = !DILocation(line: 20, column: 5, scope: !36)
!36 = distinct !DILexicalBlock(scope: !29, file: !3, line: 19, column: 30)
!37 = !DILocalVariable(name: "infn", scope: !16, file: !3, line: 24, type: !21)
!38 = !DILocation(line: 24, column: 10, scope: !16)
!39 = !DILocalVariable(name: "ansfn", scope: !16, file: !3, line: 25, type: !21)
!40 = !DILocation(line: 25, column: 10, scope: !16)
!41 = !DILocation(line: 26, column: 7, scope: !42)
!42 = distinct !DILexicalBlock(scope: !16, file: !3, line: 26, column: 7)
!43 = !DILocation(line: 26, column: 12, scope: !42)
!44 = !DILocation(line: 26, column: 7, scope: !16)
!45 = !DILocation(line: 27, column: 12, scope: !46)
!46 = distinct !DILexicalBlock(scope: !42, file: !3, line: 26, column: 18)
!47 = !DILocation(line: 27, column: 10, scope: !46)
!48 = !DILocation(line: 28, column: 13, scope: !46)
!49 = !DILocation(line: 28, column: 11, scope: !46)
!50 = !DILocation(line: 29, column: 3, scope: !46)
!51 = !DILocalVariable(name: "in", scope: !16, file: !3, line: 31, type: !52)
!52 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !53, size: 64)
!53 = !DIDerivedType(tag: DW_TAG_typedef, name: "FILE", file: !54, line: 48, baseType: !55)
!54 = !DIFile(filename: "/usr/include/stdio.h", directory: "")
!55 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_IO_FILE", file: !56, line: 241, size: 1728, elements: !57)
!56 = !DIFile(filename: "/usr/include/libio.h", directory: "")
!57 = !{!58, !59, !60, !61, !62, !63, !64, !65, !66, !67, !68, !69, !70, !78, !79, !80, !81, !85, !87, !89, !93, !96, !98, !100, !101, !102, !103, !107, !108}
!58 = !DIDerivedType(tag: DW_TAG_member, name: "_flags", scope: !55, file: !56, line: 242, baseType: !19, size: 32)
!59 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_ptr", scope: !55, file: !56, line: 247, baseType: !21, size: 64, offset: 64)
!60 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_end", scope: !55, file: !56, line: 248, baseType: !21, size: 64, offset: 128)
!61 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_read_base", scope: !55, file: !56, line: 249, baseType: !21, size: 64, offset: 192)
!62 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_base", scope: !55, file: !56, line: 250, baseType: !21, size: 64, offset: 256)
!63 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_ptr", scope: !55, file: !56, line: 251, baseType: !21, size: 64, offset: 320)
!64 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_write_end", scope: !55, file: !56, line: 252, baseType: !21, size: 64, offset: 384)
!65 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_buf_base", scope: !55, file: !56, line: 253, baseType: !21, size: 64, offset: 448)
!66 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_buf_end", scope: !55, file: !56, line: 254, baseType: !21, size: 64, offset: 512)
!67 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_save_base", scope: !55, file: !56, line: 256, baseType: !21, size: 64, offset: 576)
!68 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_backup_base", scope: !55, file: !56, line: 257, baseType: !21, size: 64, offset: 640)
!69 = !DIDerivedType(tag: DW_TAG_member, name: "_IO_save_end", scope: !55, file: !56, line: 258, baseType: !21, size: 64, offset: 704)
!70 = !DIDerivedType(tag: DW_TAG_member, name: "_markers", scope: !55, file: !56, line: 260, baseType: !71, size: 64, offset: 768)
!71 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !72, size: 64)
!72 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "_IO_marker", file: !56, line: 156, size: 192, elements: !73)
!73 = !{!74, !75, !77}
!74 = !DIDerivedType(tag: DW_TAG_member, name: "_next", scope: !72, file: !56, line: 157, baseType: !71, size: 64)
!75 = !DIDerivedType(tag: DW_TAG_member, name: "_sbuf", scope: !72, file: !56, line: 158, baseType: !76, size: 64, offset: 64)
!76 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !55, size: 64)
!77 = !DIDerivedType(tag: DW_TAG_member, name: "_pos", scope: !72, file: !56, line: 162, baseType: !19, size: 32, offset: 128)
!78 = !DIDerivedType(tag: DW_TAG_member, name: "_chain", scope: !55, file: !56, line: 262, baseType: !76, size: 64, offset: 832)
!79 = !DIDerivedType(tag: DW_TAG_member, name: "_fileno", scope: !55, file: !56, line: 264, baseType: !19, size: 32, offset: 896)
!80 = !DIDerivedType(tag: DW_TAG_member, name: "_flags2", scope: !55, file: !56, line: 268, baseType: !19, size: 32, offset: 928)
!81 = !DIDerivedType(tag: DW_TAG_member, name: "_old_offset", scope: !55, file: !56, line: 270, baseType: !82, size: 64, offset: 960)
!82 = !DIDerivedType(tag: DW_TAG_typedef, name: "__off_t", file: !83, line: 131, baseType: !84)
!83 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/types.h", directory: "")
!84 = !DIBasicType(name: "long int", size: 64, encoding: DW_ATE_signed)
!85 = !DIDerivedType(tag: DW_TAG_member, name: "_cur_column", scope: !55, file: !56, line: 274, baseType: !86, size: 16, offset: 1024)
!86 = !DIBasicType(name: "unsigned short", size: 16, encoding: DW_ATE_unsigned)
!87 = !DIDerivedType(tag: DW_TAG_member, name: "_vtable_offset", scope: !55, file: !56, line: 275, baseType: !88, size: 8, offset: 1040)
!88 = !DIBasicType(name: "signed char", size: 8, encoding: DW_ATE_signed_char)
!89 = !DIDerivedType(tag: DW_TAG_member, name: "_shortbuf", scope: !55, file: !56, line: 276, baseType: !90, size: 8, offset: 1048)
!90 = !DICompositeType(tag: DW_TAG_array_type, baseType: !22, size: 8, elements: !91)
!91 = !{!92}
!92 = !DISubrange(count: 1)
!93 = !DIDerivedType(tag: DW_TAG_member, name: "_lock", scope: !55, file: !56, line: 280, baseType: !94, size: 64, offset: 1088)
!94 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !95, size: 64)
!95 = !DIDerivedType(tag: DW_TAG_typedef, name: "_IO_lock_t", file: !56, line: 150, baseType: null)
!96 = !DIDerivedType(tag: DW_TAG_member, name: "_offset", scope: !55, file: !56, line: 289, baseType: !97, size: 64, offset: 1152)
!97 = !DIDerivedType(tag: DW_TAG_typedef, name: "__off64_t", file: !83, line: 132, baseType: !84)
!98 = !DIDerivedType(tag: DW_TAG_member, name: "__pad1", scope: !55, file: !56, line: 297, baseType: !99, size: 64, offset: 1216)
!99 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: null, size: 64)
!100 = !DIDerivedType(tag: DW_TAG_member, name: "__pad2", scope: !55, file: !56, line: 298, baseType: !99, size: 64, offset: 1280)
!101 = !DIDerivedType(tag: DW_TAG_member, name: "__pad3", scope: !55, file: !56, line: 299, baseType: !99, size: 64, offset: 1344)
!102 = !DIDerivedType(tag: DW_TAG_member, name: "__pad4", scope: !55, file: !56, line: 300, baseType: !99, size: 64, offset: 1408)
!103 = !DIDerivedType(tag: DW_TAG_member, name: "__pad5", scope: !55, file: !56, line: 302, baseType: !104, size: 64, offset: 1472)
!104 = !DIDerivedType(tag: DW_TAG_typedef, name: "size_t", file: !105, line: 62, baseType: !106)
!105 = !DIFile(filename: "llvm/lib/clang/9.0.0/include/stddef.h", directory: "/home/eli")
!106 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!107 = !DIDerivedType(tag: DW_TAG_member, name: "_mode", scope: !55, file: !56, line: 303, baseType: !19, size: 32, offset: 1536)
!108 = !DIDerivedType(tag: DW_TAG_member, name: "_unused2", scope: !55, file: !56, line: 305, baseType: !109, size: 160, offset: 1568)
!109 = !DICompositeType(tag: DW_TAG_array_type, baseType: !22, size: 160, elements: !110)
!110 = !{!111}
!111 = !DISubrange(count: 20)
!112 = !DILocation(line: 31, column: 10, scope: !16)
!113 = !DILocation(line: 31, column: 21, scope: !16)
!114 = !DILocation(line: 31, column: 15, scope: !16)
!115 = !DILocation(line: 32, column: 8, scope: !116)
!116 = distinct !DILexicalBlock(scope: !16, file: !3, line: 32, column: 7)
!117 = !DILocation(line: 32, column: 7, scope: !16)
!118 = !DILocation(line: 32, column: 12, scope: !116)
!119 = !DILocalVariable(name: "ans", scope: !16, file: !3, line: 33, type: !52)
!120 = !DILocation(line: 33, column: 10, scope: !16)
!121 = !DILocation(line: 33, column: 22, scope: !16)
!122 = !DILocation(line: 33, column: 16, scope: !16)
!123 = !DILocation(line: 34, column: 8, scope: !124)
!124 = distinct !DILexicalBlock(scope: !16, file: !3, line: 34, column: 7)
!125 = !DILocation(line: 34, column: 7, scope: !16)
!126 = !DILocation(line: 34, column: 13, scope: !124)
!127 = !DILocalVariable(name: "line_num", scope: !16, file: !3, line: 36, type: !19)
!128 = !DILocation(line: 36, column: 7, scope: !16)
!129 = !DILocalVariable(name: "in_line", scope: !16, file: !3, line: 37, type: !21)
!130 = !DILocation(line: 37, column: 10, scope: !16)
!131 = !DILocalVariable(name: "line_tmp", scope: !16, file: !3, line: 37, type: !21)
!132 = !DILocation(line: 37, column: 28, scope: !16)
!133 = !DILocalVariable(name: "in_pos", scope: !16, file: !3, line: 38, type: !104)
!134 = !DILocation(line: 38, column: 10, scope: !16)
!135 = !DILocalVariable(name: "end", scope: !16, file: !3, line: 39, type: !21)
!136 = !DILocation(line: 39, column: 10, scope: !16)
!137 = !DILocalVariable(name: "n_A", scope: !16, file: !3, line: 40, type: !19)
!138 = !DILocation(line: 40, column: 7, scope: !16)
!139 = !DILocalVariable(name: "n", scope: !16, file: !3, line: 41, type: !19)
!140 = !DILocation(line: 41, column: 7, scope: !16)
!141 = !DILocalVariable(name: "x", scope: !16, file: !3, line: 42, type: !142)
!142 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !143, size: 64)
!143 = !DIBasicType(name: "double", size: 64, encoding: DW_ATE_float)
!144 = !DILocation(line: 42, column: 12, scope: !16)
!145 = !DILocalVariable(name: "A", scope: !16, file: !3, line: 44, type: !142)
!146 = !DILocation(line: 44, column: 12, scope: !16)
!147 = !DILocation(line: 44, column: 16, scope: !16)
!148 = !DILocalVariable(name: "B", scope: !16, file: !3, line: 45, type: !142)
!149 = !DILocation(line: 45, column: 12, scope: !16)
!150 = !DILocation(line: 45, column: 16, scope: !16)
!151 = !DILocalVariable(name: "max_len", scope: !16, file: !3, line: 46, type: !19)
!152 = !DILocation(line: 46, column: 7, scope: !16)
!153 = !DILocation(line: 48, column: 3, scope: !16)
!154 = !DILocation(line: 48, column: 37, scope: !16)
!155 = !DILocation(line: 48, column: 10, scope: !16)
!156 = !DILocation(line: 48, column: 41, scope: !16)
!157 = !DILocation(line: 49, column: 13, scope: !158)
!158 = distinct !DILexicalBlock(scope: !16, file: !3, line: 48, column: 46)
!159 = !DILocation(line: 50, column: 16, scope: !158)
!160 = !DILocation(line: 50, column: 14, scope: !158)
!161 = !DILocation(line: 52, column: 9, scope: !162)
!162 = distinct !DILexicalBlock(scope: !158, file: !3, line: 52, column: 9)
!163 = !DILocation(line: 52, column: 15, scope: !162)
!164 = !DILocation(line: 52, column: 9, scope: !158)
!165 = !DILocation(line: 53, column: 11, scope: !166)
!166 = distinct !DILexicalBlock(scope: !162, file: !3, line: 52, column: 24)
!167 = !DILocation(line: 53, column: 9, scope: !166)
!168 = !DILocation(line: 54, column: 13, scope: !166)
!169 = !DILocation(line: 55, column: 5, scope: !166)
!170 = !DILocation(line: 55, column: 16, scope: !171)
!171 = distinct !DILexicalBlock(scope: !162, file: !3, line: 55, column: 16)
!172 = !DILocation(line: 55, column: 22, scope: !171)
!173 = !DILocation(line: 55, column: 16, scope: !162)
!174 = !DILocation(line: 56, column: 13, scope: !175)
!175 = distinct !DILexicalBlock(scope: !171, file: !3, line: 55, column: 31)
!176 = !DILocation(line: 56, column: 11, scope: !175)
!177 = !DILocation(line: 57, column: 11, scope: !175)
!178 = !DILocation(line: 57, column: 9, scope: !175)
!179 = !DILocation(line: 58, column: 13, scope: !175)
!180 = !DILocation(line: 59, column: 5, scope: !175)
!181 = !DILocation(line: 60, column: 11, scope: !182)
!182 = distinct !DILexicalBlock(scope: !183, file: !3, line: 60, column: 11)
!183 = distinct !DILexicalBlock(scope: !171, file: !3, line: 59, column: 12)
!184 = !DILocation(line: 60, column: 23, scope: !182)
!185 = !DILocation(line: 60, column: 11, scope: !183)
!186 = !DILocation(line: 62, column: 13, scope: !187)
!187 = distinct !DILexicalBlock(scope: !188, file: !3, line: 62, column: 13)
!188 = distinct !DILexicalBlock(scope: !182, file: !3, line: 60, column: 31)
!189 = !DILocation(line: 62, column: 18, scope: !187)
!190 = !DILocation(line: 62, column: 15, scope: !187)
!191 = !DILocation(line: 62, column: 13, scope: !188)
!192 = !DILocation(line: 63, column: 11, scope: !193)
!193 = distinct !DILexicalBlock(scope: !187, file: !3, line: 62, column: 23)
!194 = !DILocation(line: 65, column: 40, scope: !188)
!195 = !DILocation(line: 65, column: 43, scope: !188)
!196 = !DILocation(line: 65, column: 49, scope: !188)
!197 = !DILocation(line: 65, column: 29, scope: !188)
!198 = !DILocation(line: 65, column: 70, scope: !188)
!199 = !DILocation(line: 65, column: 73, scope: !188)
!200 = !DILocation(line: 65, column: 58, scope: !188)
!201 = !DILocation(line: 65, column: 93, scope: !188)
!202 = !DILocation(line: 65, column: 96, scope: !188)
!203 = !DILocation(line: 65, column: 81, scope: !188)
!204 = !DILocation(line: 65, column: 79, scope: !188)
!205 = !DILocation(line: 65, column: 55, scope: !188)
!206 = !DILocation(line: 65, column: 24, scope: !188)
!207 = !DILocation(line: 65, column: 9, scope: !188)
!208 = !DILocation(line: 66, column: 15, scope: !188)
!209 = !DILocation(line: 67, column: 9, scope: !188)
!210 = distinct !{!210, !153, !211}
!211 = !DILocation(line: 96, column: 3, scope: !16)
!212 = !DILocation(line: 69, column: 9, scope: !213)
!213 = distinct !DILexicalBlock(scope: !182, file: !3, line: 68, column: 14)
!214 = !DILocation(line: 73, column: 7, scope: !158)
!215 = !DILocation(line: 74, column: 5, scope: !158)
!216 = !DILocation(line: 74, column: 13, scope: !158)
!217 = !DILocation(line: 74, column: 12, scope: !158)
!218 = !DILocation(line: 74, column: 22, scope: !158)
!219 = !DILocation(line: 76, column: 21, scope: !220)
!220 = distinct !DILexicalBlock(scope: !158, file: !3, line: 74, column: 31)
!221 = !DILocation(line: 76, column: 14, scope: !220)
!222 = !DILocation(line: 76, column: 7, scope: !220)
!223 = !DILocation(line: 76, column: 9, scope: !220)
!224 = !DILocation(line: 76, column: 12, scope: !220)
!225 = !DILocation(line: 78, column: 11, scope: !226)
!226 = distinct !DILexicalBlock(scope: !220, file: !3, line: 78, column: 11)
!227 = !DILocation(line: 78, column: 18, scope: !226)
!228 = !DILocation(line: 78, column: 15, scope: !226)
!229 = !DILocation(line: 78, column: 11, scope: !220)
!230 = !DILocation(line: 79, column: 9, scope: !226)
!231 = !DILocation(line: 81, column: 11, scope: !232)
!232 = distinct !DILexicalBlock(scope: !220, file: !3, line: 81, column: 11)
!233 = !DILocation(line: 81, column: 18, scope: !232)
!234 = !DILocation(line: 81, column: 15, scope: !232)
!235 = !DILocation(line: 81, column: 11, scope: !220)
!236 = !DILocation(line: 83, column: 17, scope: !237)
!237 = distinct !DILexicalBlock(scope: !232, file: !3, line: 81, column: 27)
!238 = !DILocation(line: 84, column: 21, scope: !237)
!239 = !DILocation(line: 84, column: 24, scope: !237)
!240 = !DILocation(line: 84, column: 32, scope: !237)
!241 = !DILocation(line: 84, column: 13, scope: !237)
!242 = !DILocation(line: 84, column: 11, scope: !237)
!243 = !DILocation(line: 85, column: 21, scope: !237)
!244 = !DILocation(line: 85, column: 24, scope: !237)
!245 = !DILocation(line: 85, column: 32, scope: !237)
!246 = !DILocation(line: 85, column: 13, scope: !237)
!247 = !DILocation(line: 85, column: 11, scope: !237)
!248 = !DILocation(line: 88, column: 14, scope: !237)
!249 = !DILocation(line: 88, column: 20, scope: !237)
!250 = !DILocation(line: 88, column: 13, scope: !237)
!251 = !DILocation(line: 88, column: 31, scope: !237)
!252 = !DILocation(line: 88, column: 35, scope: !237)
!253 = !DILocation(line: 88, column: 11, scope: !237)
!254 = !DILocation(line: 89, column: 7, scope: !237)
!255 = !DILocation(line: 91, column: 18, scope: !220)
!256 = !DILocation(line: 91, column: 16, scope: !220)
!257 = !DILocation(line: 92, column: 7, scope: !220)
!258 = !DILocation(line: 92, column: 15, scope: !220)
!259 = !DILocation(line: 92, column: 13, scope: !220)
!260 = !DILocation(line: 92, column: 27, scope: !220)
!261 = distinct !{!261, !257, !262}
!262 = !DILocation(line: 92, column: 34, scope: !220)
!263 = distinct !{!263, !215, !264}
!264 = !DILocation(line: 94, column: 5, scope: !158)
!265 = !DILocation(line: 97, column: 1, scope: !16)
