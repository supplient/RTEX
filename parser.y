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
	#include <stdio.h>
	#include <iostream>
	#include <string>
	#include <vector>
	#include <stdint.h>
	#include <cmath>

    #define YYDEBUG 1

	using namespace std;

	/*避免包含头文件时冲突*/
	namespace rtex {
		class Scanner;
		class Driver;
	}

	enum NUMBER_TYPE {
		TYPE_REAL,
		TYPE_INTEGER
	};

	struct NUMBER {
        union {
            double real;
            int integer;
        } value;
		NUMBER_TYPE type;
    };

    typedef vector<NUMBER> NUMBER_LIST;

    struct VAR {
        union {
            struct NUMBER num;
            NUMBER_LIST list;
        } value;
        enum {
            NUMBER,
            LIST
        } type;
    };
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
%token CARET                "^"

%token EOF 0
%token LINE_CHANGE LET SUM ELLIPSIS IF ELSE
%token <string> TYPE
%token <char> NUMBER_OPERATOR
%token <string> BUILT_IN_FUNCTION
%token <double> REAL
%token <int> INTEGER
%token <string> IDENTIFIER

%type <struct NUMBER> number-expression
%type <struct NUMBER> one-character-var
%type <NUMBER_LIST> number-sequence
%type <NUMBER_LIST> subscript-dim
 
 
%start statements
 
%%
statements: /* empty */
| statements LINE_CHANGE statement
| statement
;

statement: type-statement
;

type-statement: LET TYPE IDENTIFIER
{
    cout << "let [TYPE](" << $2 << ") [IDENTIFIER](" << $3 << ")"  << endl;
}
| LET TYPE IDENTIFIER subscript-dim
{
    cout << "let [TYPE](" << $2 << ") [IDENTIFIER](" << $3 << ")_{";
    for(int i=0; i<$4.size(); i++) {
        if($4[i].type == NUMBER_TYPE::TYPE_INTEGER)
            cout << $4[i].value.integer;
        else
            cout << $4[i].value.real;
        if(i != $4.size()-1)
            cout << ",";
    }
    cout << "}" << endl;
}
;

subscript-dim: "_" "{" number-sequence "}"
{
    $$ = $3;
}
| "_" one-character-var
{ 
    $$.push_back($2);
}
;

number-sequence: number-expression
{
    $$.push_back($1);
}
| number-sequence "," number-expression
{
    $$ = $1;
    $$.push_back($3);
}
;

one-character-var: INTEGER
{
    $$.type = NUMBER_TYPE::TYPE_INTEGER;
    $$.value.integer = $1;
}
| REAL
{
    $$.type = NUMBER_TYPE::TYPE_REAL;
    $$.value.real = $1;
}
/*
| IDENTIFIER
*/
;

number-expression: INTEGER
{
    $$.type = NUMBER_TYPE::TYPE_INTEGER;
    $$.value.integer = $1;
}
| REAL
{
    $$.type = NUMBER_TYPE::TYPE_REAL;
    $$.value.real = $1;
}
/*
| number-expression NUMBER_OPERATOR number-expression
| BUILT_IN_FUNCTION "(" number-expression ")"
| var-expression
| list-expression
| sum-expression
*/
;
 
%%


/*Parser实现错误处理接口*/
void rtex::Parser::error(const rtex::location& location,const std::string& message){
  std::cout<<"error happened at: "<<location<<std::endl;
}