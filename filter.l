%{
	#include <iostream>
	#include <string>
	#include <cerrno>
	#include <climits>
	#include <cstdlib>
	#include <string>
 
	#include "filter.h" //包含yyFlexLexer子类的头文件
	#define YY_USER_ACTION 
	#define yyterminate() 0
	#undef  yywrap
	#define yywrap() 1

	using namespace std;

	/*使用默认的get_next_token定义*/
	#undef YY_DECL
	#define YY_DECL int rtex::Filter::yylex()
%}

%option c++
%option noyywrap debug
/*使用Filter::yylex()*/
%option yyclass="Filter"
%option prefix="rtexFilter"

%%
%{
%}
\\@@##@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_trans();
}

@@##@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_clear();
}

\\@@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_trans();
}

\\@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_trans();
}

@@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_block();
}

@@ {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_inline();
}

<<EOF>> return yyterminate();

\n {
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_any();
}

.	{
	Filter* x = dynamic_cast<Filter*>(this);
	x->solve_any();
}

%%
//这里可以放一些C或者C++代码
