%{
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <cstring> // strerror
# include <string>

#include "rtex.tab.hh"

%}

DIGIT [0-9]
ALPHA [a-z]|[A-Z]

%option c++

%%
\n {
    return yy::parser::make_LINE_CHANGE();
}

let {
    return yy::parser::make_LET();
}

sum {
    return yy::parser::make_SUM();
}

\.\.\. {
    return yy::parser::make_ELLIPSIS();
}

if {
    return yy::parser::make_IF();
}

else {
    return yy::parser::make_ELSE();
}


Matrix|Real|Integer {
    return yy::parser::make_TYPE(strdup(YYText()));
}

\+|\-|\*|\/ {
    return yy::parser::make_NUMBER_OPERATOR(YYText()[0]);
}

cos {
    return yy::parser::make_BUILT_IN_FUNCTION(strdup(YYText()));
}


{DIGIT}+\.{DIGIT}+ {
    yylval.TOK_REAL = atof(yytext);
    return yy::parser::make_REAL(atof(YYText()));
}

{DIGIT}+ {
    return yy::parser::make_INTEGER(atoi(YYText()));
}

{ALPHA}+({DIGIT}|{ALPHA})* {
    return yy::parser::make_IDENTIFIER(strdup(YYText()));
}

[ ]+ /* Eat Blanks */

\[ {
    return (int)YYText()[0];
}

\] {
    return (int)YYText()[0];
}

. {
    return (int)YYText()[0];
}

%%