%{
	#include <iostream>
	#include <string>
	#include <cerrno>
	#include <climits>
	#include <cstdlib>
	#include <string>
 
	#include "parser.tab.h"//包含由parser.l生成的头文件
	#include "scanner.h" //包含yyFlexLexer子类的头文件
	#include "location.hh" //包含位置调试信息头文件
	static rtex::location loc;//声明位置实例
	#define YY_USER_ACTION  loc.columns (yyleng);
	#define yyterminate() rtex::Parser::make_EOF(loc);
	#undef  yywrap
	#define yywrap() 1

	using namespace std;

	/*替换默认的get_next_token定义*/
	#undef YY_DECL
	#define YY_DECL rtex::Parser::symbol_type rtex::Scanner::get_next_token()
%}

%option c++
%option noyywrap debug
/*使用Scanner::yylex()*/
%option yyclass="Scanner"
%option prefix="rtex"

DIGIT [0-9]
ALPHA [a-z]|[A-Z]


%%
%{
	/*每次parse调用时都执行一次*/
	loc.step();
%}

\n+		loc.lines(yyleng); loc.step();

@@@		return rtex::Parser::make_BLOCK_PARAGRAPH_MARK(loc);
@@		return rtex::Parser::make_INLINE_PARAGRAPH_MARK(loc);

\_		return rtex::Parser::make_UNDERSCORE(loc);
\{		return rtex::Parser::make_LEFT_BRACE(loc);
\}		return rtex::Parser::make_RIGHT_BRACE(loc);
\;		return rtex::Parser::make_SEMICOLON(loc);
\=		return rtex::Parser::make_EQUAL(loc);
\,		return rtex::Parser::make_COMMA(loc);
\(		return rtex::Parser::make_LEFT_PARENTHESE(loc);
\)		return rtex::Parser::make_RIGHT_PARENTHESE(loc);
\[		return rtex::Parser::make_LEFT_BRACKET(loc);
\]		return rtex::Parser::make_RIGHT_BRACKET(loc);
\^T		return rtex::Parser::make_TRANSPOSE(loc);
\^		return rtex::Parser::make_CARET(loc);
\#		return rtex::Parser::make_WELL(loc);

let		return rtex::Parser::make_LET(loc);
\\sum	return rtex::Parser::make_SUM(loc);
\.\.\.  return rtex::Parser::make_ELLIPSIS(loc);
if		return rtex::Parser::make_IF(loc);
else    return rtex::Parser::make_ELSE(loc);
where	return rtex::Parser::make_WHERE(loc);


Matrix|Real|Integer {
    return rtex::Parser::make_TYPE(string(yytext), loc);
}

\+|\-|\*|\/|\>|\<|\=\=|\!\=|\>\=|\<\= {
    return rtex::Parser::make_OPERATOR(string(yytext), loc);
}

cos {
    return rtex::Parser::make_BUILT_IN_FUNCTION(string(yytext), loc);
}


{DIGIT}+\.{DIGIT}+ {
    return rtex::Parser::make_REAL(atof(yytext), loc);
}

{DIGIT}+ {
    return rtex::Parser::make_INTEGER(atoi(yytext), loc);
}

{ALPHA}+({DIGIT}|{ALPHA})* {
    return rtex::Parser::make_IDENTIFIER(string(yytext), loc);
}

[ ]+ 	  loc.step();/* Eat Blanks */

<<EOF>> 	return yyterminate();

.	{
	throw rtex::Parser::syntax_error(loc, "invalid character: " + string(yytext));
}

%%
//这里可以放一些C或者C++代码
