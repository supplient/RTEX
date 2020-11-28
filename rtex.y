%{
    #include <stdio.h>
    #include <string.h>

    int yyparse();

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

%code top {
    #include <FlexLexer.h>

    // Tell Flex the lexer's prototype ...
    # define YY_DECL \
    yy::parser::symbol_type MyScanner::yylex ()
    // ... and declare it for the parser's sake.
    YY_DECL;

    class MyScanner:public yyFlexScanner {
    public:
        virtual yy::parser::symbol_type yylex();
        virtual ~MyScanner() {}
    };
}

%code requires {
    struct NUMBER {
        union {
            double real;
            int integer;
        } value;
        enum {
            REAL,
            INTEGER
        } type;
    };

    struct NUMBER_LIST {
        int count;
        struct NUMBER* a;
    };

    struct VAR {
        union {
            struct NUMBER num;
            struct NUMBER_LIST list;
        } value;
        enum {
            NUMBER,
            LIST
        } type;
    };
}

%language "c++"
%define api.token.prefix {TOK_}
%define api.value.type variant

%token LINE_CHANGE LET SUM ELLIPSIS IF ELSE
%token <char*> TYPE
%token <char> NUMBER_OPERATOR
%token <char*> BUILT_IN_FUNCTION
%token <double> REAL
%token <int> INTEGER
%token <char*> IDENTIFIER

// %type <struct POS> pos
%type <struct NUMBER> number-expression
%type <struct NUMBER> one-character-var
%type <struct NUMBER_LIST> number-sequence
%type <struct NUMBER_LIST> subscript-dim

%%
statements: /* empty */
| statements LINE_CHANGE statement
| statement
;

statement: type-statement
;

type-statement: LET TYPE IDENTIFIER
{
    printf("let [TYPE](%s) [IDENTIFIER](%s)\n", $2, $3);
}
| LET TYPE IDENTIFIER subscript-dim
{
    printf("let [TYPE](%s) [IDENTIFIER](%s)_{", $2, $3);
    for(int i=0; i<$4.count; i++) {
        if($4.a[i].type == INTEGER)
            printf("%d", $4.a[i].value.integer);
        else
            printf("%f", $4.a[i].value.real);
        if(i != $4.count-1)
            printf(",");
    }
    printf("}\n");
}
;

subscript-dim: '_' '{' number-sequence '}'
{
    $$ = $3;
}
| '_' one-character-var
{ 
    $$.count = 1;
    $$.a = (struct NUMBER*)malloc(sizeof(struct NUMBER) * $$.count);

    $$.a[0] = $2;
}
;

number-sequence: number-expression
{
    $$.count = 1;
    $$.a = (struct NUMBER*)malloc(sizeof(struct NUMBER) * $$.count);

    $$.a[0] = $1;
}
| number-sequence ',' number-expression
{
    $$.count = $1.count + 1;
    $$.a = (struct NUMBER*)malloc(sizeof(struct NUMBER) * $$.count);

    for(int i=0; i<$$.count-1; i++)
        $$.a[i] = $1.a[i];
    $$.a[$$.count-1] = $3;

    free($1.a);
}
;

one-character-var: INTEGER
{
    $$.type = INTEGER;
    $$.value.integer = $1;
}
| REAL
{
    $$.type = REAL;
    $$.value.real = $1;
}
/*
| IDENTIFIER
*/
;

number-expression: INTEGER
{
    $$.type = INTEGER;
    $$.value.integer = $1;
}
| REAL
{
    $$.type = REAL;
    $$.value.real = $1;
}
/*
| number-expression NUMBER_OPERATOR number-expression
| BUILT_IN_FUNCTION '(' number-expression ')'
| var-expression
| list-expression
| sum-expression
*/
;