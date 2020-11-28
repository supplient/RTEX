%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kkk.tab.h"
%}

DIGIT [0-9]

%%
{DIGIT}+ {
    yylval.TOK_NUMBER = atoi(yytext);
    return TOK_NUMBER;
}

draw|move {
    yylval.TOK_COMMAND = strdup(yytext);
    return TOK_COMMAND;
}

circle {
    yylval.TOK_PRIMITIVE = strdup(yytext);
    return TOK_PRIMITIVE;
}
to {
    yylval.TOK_DIRECTION = strdup(yytext);
    return TOK_DIRECTION;
}

[ ]+ /* Eat Blanks*/
. {
    return (int)yytext[0];
}

%%