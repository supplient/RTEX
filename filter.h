#ifndef FILTER_H
#define FILTER_H

#include <iostream>
#include "driver.h"

/*重要*/
#if defined(SCANNER_FLEX) || !defined(yyFlexLexer)
    #undef yyFlexLexer
    #define yyFlexLexer rtexFilterFlexLexer  //根据prefix修改
    #include <FlexLexer.h>
#endif

using namespace std;
 
namespace rtex {
    class Filter: public yyFlexLexer 
    {
    private:
        istream& in;
        ostream& out;
        ostream& err;

    public:
        Driver driver;
        stringstream ss;

        bool isBlock = false;
        bool isInline = false;

        Filter(istream& in, ostream& out, ostream& err)
            :in(in), out(out), err(err), driver(ss, out, err)
        {
            switch_streams(in, out);
        }
        virtual int yylex();
        virtual ~Filter(){}

        void solve_clear() {
            driver.clear();
        }

        void solve_block() {
            if(isBlock) {
                isBlock = false;
                ss << yytext;
                driver.parse();
            }
            else {
                isBlock = true;
                ss << yytext;
            }
        }

        void solve_inline() {
            if(isInline) {
                isInline = false;
                ss << yytext;
                driver.parse();
            }
            else {
                isInline = true;
                ss << yytext;
            }
        }

        void solve_any() {
            if(isInline || isBlock)
                ss << yytext;
            else
                out << yytext;
        }

        void solve_trans() {
            if(isInline || isBlock)
                ss << yytext + 1;
            else
                out << yytext + 1;
        }
    };
} /* rtex */
 
 
#endif