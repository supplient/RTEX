#ifndef SCANNER_HH
#define SCANNER_HH

#if !defined(yyFlexLexer)
  #define SCANNER_FLEX
  #define yyFlexLexer rtexFlexLexer  //根据prefix修改
  #include <FlexLexer.h>
#endif
 
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