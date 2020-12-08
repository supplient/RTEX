# 语义域
| 含义 | 记号 | 定义 |
| ---  | --- | --- |
| 整数域 | Integer | - |
| 实数域 | Real | - |
| 字符域 | Character | - |
| 布尔域 | Bool | - |
| ---  | --- | --- |
| 整数列表域 | IntList | Integer -> Integer |
| 实数列表域 | RealList | Integer -> Real |
| 字符列表域 | CharList | Integer -> Character |
| 布尔列表域 | BoolList | Integer -> Bool |
| 矩阵域 | Matrix | Integer X Integer -> Real |
| 字符串域 | String | CharList |
| 字符串列表域 | StrList | Integer -> String |
| 语段列表域 | PhaseList | Integer -> Phase |
| 右值函数列表域 | RightValueFuncList | Integer -> RightValueFunc |
| 列表值域 | ListValue | Integer + Real + Character + Bool + String + Phase + RightValueFunc |
| 列表域 | List | IntList + RealList + CharList + BoolList + StrList + PhaseList + RightValueFuncList|
| ---  | --- | --- |
| 变量类型域 | Type | {"Integer", "Real", "Matrix"} |
| 变量地址域 | Location | Integer |
| 变量域 | Var | Type X Location |
| ---  | --- | --- |
| 符号表域 | SymTbl | String -> Var |
| 整数变量堆域 | IntHeap | Location -> Integer |
| 实数变量堆域 | RealHeap | Location -> Real |
| 矩阵变量堆域 | MatHeap | Location -> Matrix |
| 输出域 | Out | String |
| 执行上下文域 | ExecContext | SymTbl X IntHeap X RealHeap X MatHeap|
| 上下文域 | Context | ExecContext X Out |
| ---  | --- | --- |
| 右值域 | RightValue | Integer + Real + Matrix |
| 左值域 | LeftValue | Var + Location X RightValueFunc X RightValueFunc |
| 循环条件域 | LoopCondition | Integer X Integer X Integer |
| ---  | --- | --- |
| 过程 | Procedure | ExecContext -> ExecContext |
| 高阶函数 | FuncForFunc | Procedure -> Procedure |
| 左值函数 | LeftValueFunc | ExecContext -> LeftValue |
| 右值函数 | RightValueFunc | ExecContext -> RightValue |
| 布尔函数 | BoolFunc | ExecContext -> Bool |
| 循环条件函数 | LoopConditionFunc | ExecContent -> LoopCondition |
| 语段 | Phase | Procedure + FuncForFunc |
| 条件函数对 | IfPair | RightValueFunc X BoolFunc |

# 辅助函数
部分函数在此处只列出而不做定义。

| 名字 | 映射 | 含义 |
| --- | --- | --- |
| ListEmpty | ->List | 返回一个空列表 |
| ListInit | ListValue->List | 返回一个仅将输入作为元素的列表 |
| ListIsEmpty | List->Bool | 若列表为空返回True，否则返回False|
| ListHead | List->ListValue | 返回列表的第一个元素 |
| ListNoHead | List->List | 返回列表除第一个元素以外剩下的部分 |
| ListTail | List->ListValue | 返回列表的最后一个元素 |
| ListNoTail | List->List | 返回列表除最后一个元素以外剩下的部分 |
| ListPushHead | ListValue X List -> List | 在函数头插入一个元素|
| ListPushTail | ListValue X List -> List | 在函数尾插入一个元素|
| ToString | RightValue -> String | 将右值转换成格式正确的字符串|
| ToMatrixString | Integer X Integer X StrList -> String | 把一组字符串排版成矩阵的形式 |
| ListToMatrix | Integer X Integer X RealList -> Matrix | 将一个实数列表转换成矩阵 |
| MartrixEmpty | Integer X Integer -> Matrix | 输入行列数，返回对应大小的空矩阵 |
| MatrixVary | Integer X Integer X Real X Matrix -> Matrix | 将指定矩阵的指定位置替换为指定值|
| StringListJoin | StringList X String -> String | 将字符串列表拼接起来 |
| OperatorCalculate | String X RightValue X RightValue -> RightValue | 会根据操作符和操作数进行适当的类型转换和计算 |
| BuiltinCalculate | String X RightValue -> RightValue | 会根据指定的函数名和操作数进行适当的类型转换和计算 |


```
register_var: SymTbl X String X Var -> SymTbl
* register_var(symTbl, id, var)(id_toSearch) = 
    if id == id_toSearch then var
    else symTbl(id_toSearch)

unregister_var: SymTbl X String -> SymTbl
* register_var(symTbl, id)(id_toSearch) = 
    if id == id_toSearch then fail
    else symTbl(id_toSearch)

allocate_int: IntHeap -> Integer X IntHeap
* let (i, intHeap') = allocate_int(intHeap) in
    intHeap'(i') = 
        if i'!=i
        then intHeap(i')
        else 0
allocate_real也是同理，不做重复

allocate_mat: MatHeap X Integer X Integer -> Integer X MatHeap
* let (i, matHeap') = allocate_mat(matHeap, row, col) in
    matHeap'(i') = 
        if i'!=i
        then matHeap(i')
        else MatrixHeap(row, col)

deallocate_int: Integer X IntHeap -> IntHeap)
* deallocate_int(i, intHeap)(i') = 
    if i'==i
    then fail
    else intHeap(i)
deallocate_real和deallocate_matrix也是同理

vary_int: Integer X Value X IntHeap -> IntHeap
* vary_int(i, v, intHeap)(i') =
    if i==i'
    then v
    else intHeap(i')
vary_real和vary_matrix也是同理

get_var(var Var, execCtx) =
    let (type, loc) = var in
    if type == "Integer" then
        intHeap(loc)
    else if type == "Real" then
        realHeap(loc)
    else if type == "Matrix" then
        matHeap(loc)
    else fail

make_var("Integer", id, execCtx) = 
    let (i, intHeap') = allocate_int(intHeap) in
    (
        register_var(symTbl, id, ("Integer",i)), 
        intHeap', realHeap, matHeap
    )
make_var("Real", id, execCtx)也是同理

remove_var("Integer", id, execCtx) = 
    let (type, i) = symTbl(id) in
    (
        unregister_var(symTbl, id), 
        deallocate_int(i, intHeap),
        realHeap, matHeap
    )
remove_var("Real", id, execCtx)同理

make_var("Matrix", id, row, col, execCtx) = 
    let (i, matHeap') = allocate_mat(matHeap, row, col) in
    (
        register_var(symTbl, id, ("Matrix",i)), 
        intHeap, realHeap, matHeap'
    )


change_var(id String, newValue, execCtx) =
    change_var(symTbl(id), newValue, execCtx)

change_var(var Var, newValue, execCtx) =
    let (type, loc) = var in
    if type=="Integer" then 
        (
            symTbl,
            vary_int(loc, newValue, intHeap),
            realHeap, matHeap
        )
    else if type=="Real" then 
        (
            symTbl,
            intHeap,
            vary_real(loc, newValue, realHeap),
            matHeap
        )
    else if type=="Matrix" then 
        (
            symTbl,
            intHeap,
            realHeap,
            vary_mat(loc, newValue, matHeap),
        )
    else fail


print_paragraph: Out X StrList X Bool -> PrintContext
* print_paragraph(out, strList, flag) = 
    let loop_print(strList, flag) = 
        if ListIsEmpty(strList) then ""
        else 
           ListHead(strList)
            + if flag then "\n" else " "
            + loop_print(out, ListNoHead(strList), flag)
        in
    (out + if flag then "$$\n" else "$"
        + loop_print(strList, flag)
        + if flag then "$$\n" else "$"
    , flag)
```


# 指称语义
设函数族$work[sentence]: Context -> Context$，即，设有$ctx \in Context$，则对任意$sentence$，都有$work[sentence] \in Context$。其中，sentence为语法元素构成的序列，语法元素构成的全集即为“附录一、语法”中定义的终止符与非终止符的集合。

而函数族$exec[sentence]$，其输入输出根据$sentence$而发生改变，我将单独列出。

为方便起见，假设输入的$ctx = (execCtx, out), execCtx = (symTbl, intHeap, realHeap, matHeap)$。

下文中将在不会引起歧义的地方略去参数。

```
work[program]
* work[paragraphs] = work[paragraphs]

paragraphs
* work[paragraph paragraphs] = work[paragraphs](work[paragraph](ctx))
* work[paragraph] = work[paragraph]

paragraph
* work[block_paragraph] = work[block_paragraph]
* work[inline_paragraph] = work[inline_paragraph]

block_paragraph
* work["@@@" statements "@@@"] = 
    let (execCtx', strList) = exec[statements](execCtx) in
    (execCtx', print_paragraph(out, strList, True))

inline_paragraph
* work["@@" statements "@@"] = 
    let (execCtx', strList) = exec[statements](execCtx) in
    (execCtx', print_paragraph(out, strList, False))

从这里开始，在语法元素旁边我会标出该语法元素对应的exec函数的映射关系

statements: ExecContext -> ExecContext X StrList
* exec[statement ";" statements] = 
    let (execCtx', s) = exec[statement](execCtx) in
    let (execCtx'', strList) = exec[statements](execCtx') in
    (execCtx'', ListPushHead(s, strList))
* exec[statement ";"] = 
    let (execCtx', s) = exec[statement](execCtx) in
    (execCtx', ListInit(s))

statement: ExecContext -> ExecContext X String
* exec[phases] = 
    let (phaseList, s) = exec[phases] in
    let 
        exec_phase(phaseList) = 
            exec_phase(ListNoTail(phaseList), ListTail(phaseList))
        exec_phase(phaseList, fff FuncForFunc) =
            fff(exec_phase(phaseList))
        exec_phase(phaseList, proc Procedure) =
            λ execCtx. exec_phase(phaseList)(proc(execCtx))
        in
    (exec_phase(phaseList)(execCtx), s)
* exec[right_exp] = 
    let rightF = exec[right_exp] in
    let rv = rightF(execCtx) in
    (execCtx, ToString(rv))


phases: -> PhaseList X String
* exec[phase "#" phases] =
    let (p, s) = exec[phase] in
    let (ps, ss) = exec[phases] in
    (ListPushHead(p, ps), s + " \\quad" + ss)
* exec[phase] =
    let (p, s) = exec[phase] in
    (ListInit(p), s)


phase: -> Phase X String
* exec[type_phase] = exec[type_phase]
* exec[assign_phase] = exec[assign_phase]
* exec[where_phase] = exec[where_phase]

type_phase: -> Procedure X String
* exec[LET TYPE IDENTIFIER] =
    let f = λ execCtx.
        make_var(TYPE, IDENTIFIER, execCtx)
    in
    (f, TYPE + "\\quad " + IDENTIFIER)
* exec[LET TYPE IDENTIFIER subscript_dim] =
    let (row_f, col_f, dim_str_list) = exec[subscript_dim] in
    let f = λ execCtx.
        let row = row_f(execCtx) in
        let col = col_f(execCtx) in
        make_var(TYPE, IDENTIFIER, row, col, execCtx)
    in
    (f, TYPE + "\\quad " + IDENTIFIER + "_{" + StringListJoin(dim_str_list, " \\times ") + "})


where_phase: -> FuncForFunc X String
* exec[WHERE IDENTIFIER "=" ellipsis_exp] =
    let loop(start, step, end, execCtx, proc) = 
        let 
            execCtx' = change_var(IDENTIFIER, start, execCtx)
        in
        if start==end then execCtx
        else loop(start+step, step, end, proc(execCtx'), proc)
    in
    let (startF, stepF, endF, elli_s) = exec[ellipsis_exp] in
    let f = λ proc. λ execCtx.
        let start = startF(execCtx) in
        let step = stepF(execCtx) in
        let end = endF(execCtx) in
        let execCtx' = make_var("Integer", IDENTIFIER, execCtx) in
        let execCtx'' = loop(start, step, end, execCtx', proc) in
        remove_var("Integer", IDENTIFIER, execCtx'')
    in
    let s = IDENTIFIER + "=" + elli_s in
    (f, s)

ellipsis_exp: -> LoopConditionFunc X String
* exec[right_exp_1 "," right_exp_2 ELLIPSIS right_exp_3] =
    let (rf1, s1) = exec[right_exp_1] in
    let (rf2, s2) = exec[right_exp_2] in
    let (rf3, s3) = exec[right_exp_3] in
    let stepF = λ execCtx.
        rf2(execCtx) - rf1(execCtx)
    (
        rf1, stepF, rf3,
        s1 + "," + s2 + "..." + s3
    )

assign_phase: -> Procedure X String
* exec[left_exp "=" right_exp] =
    let 
        assign(var Var, rv, execCtx) = change_var(var, rv, execCtx)
        assign(loc Integer, rowF RightValueFunc, colF RightValueFunc, rv, execCtx) =
            let row = rowF(execCtx) in
            let col = colF(execCtx) in
            change_var(
                ("Matrix", loc),
                MatrixVary(
                    row, col,
                    rv,
                    matTbl(loc)
                ),
                execCtx
            )
    in
    let (lf, ls) = exec[left_exp] in
    let (rf, rs) = exec[right_exp] in
    let f = λ execCtx.
        let lv = lf(execCtx) in
        let rv = rf(execCtx) in
        assign(lv, rv, execCtx)
    in
    (f, ls + " = " + rs)

left_exp: -> LeftValueFunc X String
* exec[IDENTIFIER] =
    let f = λ execCtx.
        symTbl(IDENTIFIER)
    in
    (f, IDENTIFIER)
* exec[IDENTIFIER subscript_dim] =
    let (rowF, colF, dim_str_list) = exec[subscript_dim] in
    let f = λ execCtx.
        let (type, loc) = symTbl(IDENTIFIER) in
        (loc, rowF, colF)
    in
    (
        f, 
        IDENTIFIER + "_{" + StringListJoin(dim_str_list) + "}"
    )

right_exp: -> RightValueFunc X String
* exec[right_exp_1 OPERATOR right_exp_2] =
    let (rf1, s1) = exec[right_exp_1] in
    let (rf2, s2) = exec[right_exp_2] in
    let f = λ execCtx.
        let rv1 = rf1(execCtx) in
        let rv2 = rf2(execCtx) in
        OperatorCalculate(OPERATOR, rv1, rv2)
    in
    (
        f,
        s1 + OPERATOR + s2
    )
* exec[BUILT_IN_FUNCTION "(" right_exp ")"] =
    let (rf, s) = exec[right_exp] in
    let f = λ execCtx.
        let rv = rf(execCtx) in
        BuiltinCalculate(BUILT_IN_FUNCTION, rv)
    in
    (
        f,
        BUILT_IN_FUNCTION + "(" + s + ")"
    )
* exec[INTEGER] =
    (
        λ execCtx. INTEGER,
        ToString(INTEGER)
    )
* exec[REAL] =
    (
        λ execCtx. REAL,
        ToString(REAL)
    )
* exec[list_exp] = exec[list_exp]
* exec[sum_exp] = exec[sum_exp]
* exec["(" if_exp ")"] = exec[if_exp]
* exec["(" right_exp ")"] =
    let (rf, s) = exec[right_exp] in
    (
        rf,
        "(" + s + ")"
    )
* exec[left_exp] =
    let (lf, s) = exec[left_exp] in
    let
        get(var Var, execCtx) = get_var(var, execCtx)
        get(loc Integer, rowF Integer, colF Integer, execCtx)=
            let 
                row = rowF(execCtx) 
                col = colF(execCtx)
            in
            matTbl(loc)(row, col)
    in
    let f = λ execCtx.
        let lv = lf(execCtx) in
        get(lv, execCtx)
    in
    (
        f,
        s
    )
* exec[right_exp "^T"]
    let (rf, s) = exec[right_exp] in
    let f = λ execCtx.
        let rv = rf(execCtx) in
        BuiltinCalculate("^T", rv)
    in
    (
        f,
        s + "^T"
    )

list_exp: -> RightValueFunc
* exec["[" "(" ROW_INT "," COL_INT ")" right_exp_list "]"] =
    let (rf_list, s_list) = exec[right_exp_list] in
    let f = λ execCtx.
        let loop_cal_rv(rf_list) =
            if ListIsEmpty(rf_list) then
                ListEmpty()
            else
                ListPushHead(
                    ListHead(rf_list)(execCtx), 
                    ListNoHead(rf_list)
                )
        in
        let rv_list = loop_cal_rv(rf_list) in
        ListToMatrix(ROW_INT, COL_INT, rv_list)
    in
    let s = ToMatrixString(ROW_INT, COL_INT, s_list) in
    (f, s)

right_exp_list: -> RightValueFuncList
* exec[right_exp "," right_exp_list] = 
    let (rf, s) = exec[right_exp] in
    let (rf_list, s_list) = exec[right_exp_list] in
    (
        ListPushHead(rf, rf_list),
        ListPushHead(s, s_list)
    )
* exec[right_exp] =
    let (rf, s) = exec[right_exp] in
    (
        ListInit(rf),
        ListInit(s)
    )

sum_exp: -> RightValueFunc
* exec[SUM subscript_cond superscript_cond right_exp] =
    let loop_sum(start, end, execCtx, rf) = 
        let 
            execCtx' = change_var(IDENTIFIER, start, execCtx)
        in
        if start==end then execCtx
        else 
            rf(execCtx') + loop_sum(start+1, end, execCtx, rf)
    in
    let (rf, rs) = exec[right_exp] in
    let (id, startF, subS) = exec[subscript_cond] in
    let (endF, superS) = exec[superscript_cond] in
    let f = λ execCtx.
        let start = startF(execCtx) in
        let end = endF(execCtx) in
        let execCtx' = make_var("Integer", id, execCtx) in
        loop_sum(start, end, execCtx', rf)
    in
    let s = "\\sum" + subS + superS + " " + rs in
    (f, s)

subscript_cond: -> String X RightValueFunc X String
* exec["_" "{" IDENTIFIER "=" right_exp "}"] =
    let (rf, s) = exec[right_exp] in
    (
        (IDENTIFER, rf),
        "_{" + IDENTIFIER + "=" + s + "}"
    )

superscript_cond: -> RightValueFunc X String
* exec["^" "{" right_exp "}"] =
    let (rf, s) = exec[right_exp] in
    (
        rf,
        "^{" + s + "}"
    )

if_exp: -> RightValueFunc X String
* exec[if_exp_phases] =
    let (ifPairList, sList) = exec[if_exp_phases] in
    let f = λ execCtx.
        let loopCheck(ifPairList) =
            if ListIsEmpty(ifPairList) then
                fail
            else
                let (headBoolF, headRightF) = ListHead(ifPairList) in
                if headBoolF(execCtx) then
                    headRightF(execCtx)
                else
                    loopCheck(ListNoHead(ifPairList))
        in
        loopCheck(ifPairList)
    in
    let 
        s = StringListJoin(sList, " \n")
    in
    (f, s)


if_exp_phases: -> IfPairList X StringList
* exec[if_exp_phase "#" if_exp_phases] =
    let (ifPair, s) = exec[if_exp_phase] in
    let (ifPairList, sList) = exec[if_exp_phases] in
    (
        ListHead(ifPair, ifPairList),
        ListHead(s, sList)
    )
* exec[if_exp_phase] =
    let (ifPair, s) = exec[if_exp_phase] in
    (
        ListInit(ifPair),
        ListInit(s)
    )

if_exp_phase: -> Ifpair X String
* exec[right_exp "," IF bool_exp] =
    let (rf, rs) = exec[right_exp] in
    let (bf, bs) = exec[bool_exp] in
    (
        (rf, bf),
        rs + "&, if " bs
    )
* exec[right_exp "," ELSE] =
    let (rf, rs) = exec[right_exp] in
    (
        (rf, λ execCtx. True),
        rs + "&, else"
    )

bool_exp: -> BoolFunc
* exec[right_exp1 OPERATOR right_exp2] =
    let (rf1, rs1) = exec[right_exp1] in
    let (rf2, rs2) = exec[right_exp2] in
    let f = λ execCtx.
        OperatorCalculate(
            OPERATOR,
            rf1(execCtx),
            rf2(execCtx),
        )
    in
    (
        f,
        rs1 + OPERATOR + rs2
    )

subscript_dim: -> RightValueFunc X RightValueFunc X Strig
* exec["_" "{" right_exp1 "," right_exp2 "}"] =
    let (rf1, rs1) = exec[right_exp1] in
    let (rf2, rs2) = exec[right_exp2] in
    (
        (rf1, rf2),
        "_{" + rs1 + "," + rs2 + "}"
    )

```