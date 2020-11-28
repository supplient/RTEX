#ifndef SCANNER_HH
#define SCANNER_HH
/*重要*/
#if ! defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer yyFlexLexer  //根据prefix修改
#include <FlexLexer.h>
#endif
/*替换默认的get_next_token定义*/
#undef YY_DECL
#define YY_DECL rtex::Parser::symbol_type rtex::Scanner::get_next_token()
 
#include "parser.tab.h"
namespace rtex {
  class Driver;
  class Scanner:public yyFlexLexer {
  private:
    /* data */
    Driver& m_driver;
 
  public:
    Scanner (rtex::Driver& driver):m_driver(driver){
 
    }
    virtual rtex::Parser::symbol_type get_next_token();
    virtual ~Scanner (){}
  };
} /* rtex */
 
 
#endif