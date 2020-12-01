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
 
 
%start statements
 
%%
statements: statement ";" statements
| statement ";"
;

statement: phases
;

phases: phases"#" phase
| phase
;

phase: type-phase
| assign-phase
| where-phase
;

type-phase: LET TYPE IDENTIFIER
| LET TYPE IDENTIFIER subscript-dim
;

where-phase: WHERE IDENTIFIER "=" ellipsis-exp
;

ellipsis-exp: right-exp "," right-exp ELLIPSIS right-exp
;

assign-phase: left-exp "=" right-exp
;

left-exp: IDENTIFIER
| IDENTIFIER subscript-dim
;

right-exp: right-exp OPERATOR right-exp
| BUILT_IN_FUNCTION "(" right-exp ")"
| INTEGER
| REAL
| list-exp
| sum-exp
| if-exp
| "(" right-exp ")"
| left-exp
| right-exp "^T"
;

list-exp: "[" right-exp-list "]"
;

right-exp-list: right-exp "," right-exp-list
| right-exp
;

sum-exp: SUM subscript-cond superscript-cond right-exp
;

subscript-cond: "_" "{" assign-phase "}"
;

superscript-cond: "^" "{" right-exp "}"
;

if-exp: if-exp-phase "#" if-exp
| if-exp-phase
;

if-exp-phase: right-exp "," IF bool-exp
| right-exp "," ELSE
;

bool-exp: right-exp OPERATOR right-exp
;

subscript-dim: "_" "{" right-exp-list "}"
;
 
%%


/*Parser实现错误处理接口*/
void rtex::Parser::error(const rtex::location& location,const std::string& message){
  std::cout<<"error happened at: "<<location<<std::endl;
}