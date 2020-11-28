%{
    #include <stdio.h>
    #include <string.h>

    int yyparse();
    int yylex();

    int yywrap() {
        return 1;
    }

    void yyerror(char const* s) {
        printf("%s", s);
    }

    extern int yy_flex_debug;
    int yydebug = 0;
    
    int main(int argc, char* argv[]) {
        yy_flex_debug = 0;

        for(int i=1; i<argc; i++) {
            if(strcmp(argv[i], "-d") == 0) {
                yy_flex_debug = 1;
                yydebug = 1;
            }
        }

        yyparse();
        return 0;
    }
%}

%code requires {
    struct POS {
        int x;
        int y;
    };
}

%define api.token.prefix {TOK_}
%define api.value.type union

%token <int> NUMBER
%token <char*> COMMAND 
%token <char*> DIRECTION 
%token <char*> PRIMITIVE
%token <char> CHAR 

%type <struct POS> pos

%%
commands: /* empty */
| commands command
;

command: COMMAND PRIMITIVE pos 
{
    printf("[Command](%s) [Primitive](%s) [Pos](%d, %d)", $1, $2, $3.x, $3.y);
}
| COMMAND DIRECTION pos
{
    printf("[Command](%s) [Direction](%s) [Pos](%d, %d)", $1, $2, $3.x, $3.y);
}
;

pos: NUMBER ',' NUMBER
{
    $$.x = $1;
    $$.y = $3;
}
;