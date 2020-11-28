#ifndef DRIVER_CPP
#define DRIVER_CPP
#include "driver.h"
#include <iostream>
using namespace rtex;
using namespace std;
Driver::Driver():m_scanner(*this),m_parser(m_scanner,*this){
    m_parser.set_debug_stream(cerr);
}

int Driver::parse() {
    m_scanner.switch_streams(cin,cout);
    return m_parser.parse();
}

void Driver::setDebug(bool flag) {
    if(flag) {
        m_scanner.set_debug(1);
        m_parser.set_debug_level(1);
    }
    else {
        m_scanner.set_debug(0);
        m_parser.set_debug_level(0);
    }
}

Driver::~Driver(){
 
}
#endif