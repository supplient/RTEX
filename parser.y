%skeleton "lalr1.cc"
%require "3.0.4"
 
%define api.namespace {rtex} //声明命名空间与下面声明的类名结合使用 rtex::Parser::  在scanner.l中有体现
%define parser_class_name { Parser }
%define api.token.constructor
%define api.value.type variant //使得类型与token定义可以使用各种复杂的结构与类型
%define parse.assert  //开启断言功能
%defines  //生成各种头文件  location.hh position.hh  parser.hpp

%code requires
{
    /*requires中的内容会放在YYLTYPE与YYSTPYPE定义前*/
    #include "parser.h"
}
 
%code top
{
  /*尽可能放在parser.cpp靠近头部的地方，与requires相似*/
  #include <iostream>
  #include <algorithm>
  #include "util.h"
  #include "scanner.h"
  #include "parser.tab.h"
  #include "driver.h"
  #include "location.hh"
 
  /*注意：这里的参数由%parse-param决定*/
  static rtex::Parser::symbol_type yylex(rtex::Scanner& scanner,rtex::Driver &driver){
    return scanner.get_next_token();
  }
  using namespace rtex;
}
 
/*定义parser传给scanner的参数*/
%lex-param { rtex::Scanner& scanner }
%lex-param { rtex::Driver& driver }
/*定义driver传给parser的参数*/
%parse-param { rtex::Scanner& scanner }
%parse-param { rtex::Driver& driver }
 
%locations
/*详细显示错误信息*/
%define parse.error verbose
/*通过rtex::Parser::make_XXX(loc)给token添加前缀*/
%define api.token.prefix {TOK_}

%token UNDERSCORE           "_"
%token LEFT_BRACE           "{"
%token RIGHT_BRACE          "}"
%token SEMICOLON            ";"
%token EQUAL                "="
%token COMMA                ","
%token LEFT_PARENTHESE      "("
%token RIGHT_PARENTHESE     ")"
%token LEFT_BRACKET         "["
%token RIGHT_BRACKET        "]"
%token TRANSPOSE            "^T"
%token CARET                "^"
%token WELL                 "#"

%token EOF 0
%token LINE_CHANGE LET SUM ELLIPSIS IF ELSE WHERE
%token <string> TYPE
%token <string> OPERATOR
%token <string> BUILT_IN_FUNCTION
%token <double> REAL
%token <int> INTEGER
%token <string> IDENTIFIER

// %type <Number> number-expression
// %type <Var> one-character-var
// %type <NumberList> number-sequence
// %type <NumberList> subscript-dim
// %type <Var&> var-expression

%type <Bag<vector<Phase>>> phases
%type <Bag<Phase>> phase
%type <Bag<Procedure>> type_phase
%type <Bag<Procedure>> assign_phase
%type <Bag<FuncForFunc>> where_phase

%type <Bag<LoopConditionFunc>> ellipsis_exp

%type <Bag<LeftValueFunc>> left_exp
%type <Bag<RightValueFunc>> right_exp
%type <Bag<RightValueFunc>> list_exp
%type <Bag<vector<RightValueFunc>, vector<string>>> right_exp_list
%type <Bag<vector<RightValueFunc>, vector<string>>> subscript_dim

%type <Bag<RightValueFunc>> sum_exp
%type <Bag<pair<string, RightValueFunc>>> subscript_cond
%type <Bag<RightValueFunc>> superscript_cond

%type <Bag<RightValueFunc>> if_exp
%type <Bag<vector<IfPair>, vector<string>>> if_exp_phases
%type <Bag<IfPair>> if_exp_phase

%type <Bag<BoolFunc>> bool_exp
 
 
%start statements
 
%%
statements: statement ";" statements
| statement ";"
;

statement: phases
{
    if(driver.outputMarkdown)
        driver.out << "$$" << $1.s << "$$" << endl << endl;

    driver.solve_statement($1.v);
}
| right_exp
{
    driver.solve_statement_print($1.v);
    // DEBUG
    driver.out << endl;
}
;

phases: phase "#" phases
{
    $$.s = $1.s + " \\quad " + $3.s;

    $$.v = $3.v;
    $$.v.insert($$.v.begin(), $1.v);
}
| phase
{
    $$.s = $1.s;

    $$.v = {$1.v};
}
;

phase: type_phase
{
    $$.s = $1.s;

    $$.v.type = Phase::PROCEDURE;
    $$.v.prod = $1.v;
}
| assign_phase
{
    $$.s = $1.s;

    $$.v.type = Phase::PROCEDURE;
    $$.v.prod = $1.v;
}
| where_phase
{
    $$.s = $1.s;

    $$.v.type = Phase::FUNC_FOR_FUNC;
    $$.v.fff = $1.v;
}
;

type_phase: LET TYPE IDENTIFIER
{
    $$.s = $2 + " \\quad " + $3 + "";

    $$.v = driver.solve_type_phase($2, $3);
}
| LET TYPE IDENTIFIER subscript_dim
{
    $$.s = $2 + " \\quad " + $3;
    $$.s += string("_") + "{";
    $$.s += Util::join($4.s, " \\times ");
    $$.s += "}";

    $$.v = driver.solve_type_phase($2, $3, $4.v);
}
;

where_phase: WHERE IDENTIFIER "=" ellipsis_exp
{
    $$.s = $2 + " = " + $4.s;

    $$.v = driver.solve_where_phase($2, $4.v);
}
;

ellipsis_exp: right_exp "," right_exp ELLIPSIS right_exp
{
    $$.s = $1.s + "," + $3.s + "..." + $5.s;

    $$.v = driver.solve_ellipsis_exp($1.v, $3.v, $5.v);
}
;

assign_phase: left_exp "=" right_exp
{
    $$.s = $1.s + " = " + $3.s;

    $$.v = driver.solve_assign_phase($1.v, $3.v);
}
;

left_exp: IDENTIFIER
{
    $$.s = $1;

    $$.v = driver.solve_left_exp_var($1);
}
| IDENTIFIER subscript_dim
{
    $$.s = $1 + "_{";
    $$.s += Util::join($2.s, ",");
    $$.s += "}";

    $$.v = driver.solve_left_exp_mat($1, $2.v);
}
;

right_exp: right_exp OPERATOR right_exp
{
    $$.s = $1.s + $2 + $3.s;

    $$.v = driver.solve_right_exp_op($1.v, $3.v, $2);
}
| BUILT_IN_FUNCTION "(" right_exp ")"
{
    $$.s = $1 + "(" + $3.s + ")";

    $$.v = driver.solve_right_exp_func($3.v, $1);
}
| INTEGER
{
    $$.s = to_string($1);

    $$.v = driver.solve_right_exp_int($1);
}
| REAL
{
    $$.s = to_string($1);

    $$.v = driver.solve_right_exp_real($1);
}
| list_exp
{
    $$.s = $1.s;

    $$.v = $1.v;
}
| sum_exp
{
    $$.s = $1.s;

    $$.v = $1.v;
}
| "(" if_exp ")"
{
    $$.s = $2.s;

    $$.v = $2.v;
}
| "(" right_exp ")"
{
    $$.s = "(" + $2.s + ")";

    $$.v = $2.v;
}
| left_exp
{
    $$.s = $1.s;

    $$.v = driver.solve_right_exp_from_left_exp($1.v);
}
| right_exp "^T"
{
    $$.s = $1.s + "^T";

    $$.v = driver.solve_right_exp_transpose($1.v);
}
;

list_exp: "[" right_exp_list "]"
{
    $$.s = "[" + Util::join($2.s, ",") + "]";

    $$.v = driver.solve_list_exp_list($2.v);
}
| "[" "(" INTEGER "," INTEGER ")" right_exp_list "]"
{
    $$.s = "\
\\left[\n\
\\begin{matrix}\n";

    for(int i=0; i<$3; i++) {
        for(int j=0; j<$5; j++) {
            int li = i*$3 + j;
            $$.s += "\t" + $7.s[li];
            if(j != $5-1)
                $$.s += " & ";
        }
        if(i != $3-1)
            $$.s += " \\\\\n";
    }

    $$.s += "\n\
\\end{matrix}\n\
\\right]";


    $$.v = driver.solve_list_exp_mat($3, $5, $7.v);
}
;

right_exp_list: right_exp "," right_exp_list
{
    $$.s = $3.s;
    $$.s.insert($$.s.begin(), $1.s);

    $$.v = $3.v;
    $$.v.insert($$.v.begin(), $1.v);
}
| right_exp
{
    $$.s = {$1.s};

    $$.v = {$1.v};
}
;

sum_exp: SUM subscript_cond superscript_cond right_exp
{
    $$.s = "\\sum" + $2.s + $3.s + " " + $4.s;

    $$.v = driver.solve_sum_exp($2.v, $3.v, $4.v);
}
;

subscript_cond: "_" "{" IDENTIFIER "=" right_exp "}"
{
    $$.s = "_{" + $3 + "=" + $5.s + "}";

    $$.v = driver.solve_subscript_cond($3, $5.v);
}
;

superscript_cond: "^" "{" right_exp "}"
{
    $$.s = "^{" + $3.s + "}";

    $$.v = $3.v;
}
;

if_exp: if_exp_phases
{
    $$.s = "\
\\left\\{\n\
\\begin{aligned}\n";
    $$.s += Util::join($1.s, " \\\\\n", "\t");
    $$.s += "\n\
\\end{aligned}\n\
\\right.";

    $$.v = driver.solve_if_exp($1.v);
}
;

if_exp_phases: if_exp_phase "#" if_exp_phases
{
    $$.s = $3.s;
    $$.s.insert($$.s.begin(), $1.s);

    $$.v = $3.v;
    $$.v.insert($$.v.begin(), $1.v);
}
| if_exp_phase
{
    $$.s = {$1.s};

    $$.v = {$1.v};
}
;

if_exp_phase: right_exp "," IF bool_exp
{
    $$.s = $1.s + "&, if " + $4.s;

    $$.v = driver.solve_if_exp_phase_if($1.v, $4.v);
}
| right_exp "," ELSE
{
    $$.s = $1.s + "&, else";

    $$.v = driver.solve_if_exp_phase_else($1.v);
}
;

bool_exp: right_exp OPERATOR right_exp
{
    $$.s = $1.s + $2 + $3.s;

    $$.v = driver.solve_bool_exp($1.v, $3.v, $2);
}
;

subscript_dim: "_" "{" right_exp_list "}"
{
    $$.s = $3.s;

    $$.v = $3.v;
}
;
 
%%


/*Parser实现错误处理接口*/
void rtex::Parser::error(const rtex::location& location,const std::string& message){
    cout
        << "Error[" << location << "]: "
        << message << endl
        ;
}