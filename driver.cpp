#ifndef DRIVER_CPP
#define DRIVER_CPP
#include "driver.h"
#include <iostream>
using namespace rtex;
using namespace std;
Driver::Driver(istream& in, ostream& out, ostream& err)
    : in(in), out(out), err(err), m_scanner(*this),m_parser(m_scanner,*this)
{
    m_parser.set_debug_stream(err);
}

int Driver::parse() {
    m_scanner.switch_streams(in, out);
    int r;
    try {
        r = m_parser.parse();
    }
    catch(const char* s) {
        cerr << s << endl;
    }
    return r;
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

namespace rtex {
    RightValue rightValue_Cal_Helper(RightValue& a, RightValue& b, string op) {
        RightValue c;

        auto REAL = RightValue::Type::REAL;
        auto INTEGER = RightValue::Type::INTEGER;
        auto MATRIX = RightValue::Type::MATRIX;
        auto LIST = RightValue::Type::LIST;

        if(a.type==REAL && b.type==REAL) {
            c.type = REAL;

            if(op == "+")
                c.realValue = a.realValue + b.realValue;
            else if(op == "-")
                c.realValue = a.realValue - b.realValue;
            else if(op == "*")
                c.realValue = a.realValue * b.realValue;
            else if(op == "/")
                c.realValue = a.realValue / b.realValue;
        }
        else if((a.type==REAL&&b.type==INTEGER) || (a.type==INTEGER&&b.type==REAL) ) {
            c.type = REAL;

            if(a.type != RightValue::Type::REAL)
                a.realValue = a.intValue;
            if(b.type != RightValue::Type::REAL)
                b.realValue = b.intValue;

            if(op == "+")
                c.realValue = a.realValue + b.realValue;
            else if(op == "-")
                c.realValue = a.realValue - b.realValue;
            else if(op == "*")
                c.realValue = a.realValue * b.realValue;
            else if(op == "/")
                c.realValue = a.realValue / b.realValue;
        }
        else if(a.type == INTEGER && b.type == INTEGER) {
            c.type = INTEGER;

            if(op == "+")
                c.intValue = a.intValue + b.intValue;
            else if(op == "-")
                c.intValue = a.intValue - b.intValue;
            else if(op == "*")
                c.intValue = a.intValue * b.intValue;
            else if(op == "/")
                c.intValue = a.intValue / b.intValue;
        }
        else if(a.type == MATRIX && b.type == MATRIX) {
            c.type = MATRIX;

            if(op == "+")
                c.matValue = a.matValue + b.matValue;
            else if(op == "-")
                c.matValue = a.matValue - b.matValue;
            else if(op == "*")
                c.matValue = a.matValue * b.matValue;
            else if(op == "/") 
                throw "Invalid divide for Matrix";
        }
        else if((a.type==MATRIX&&b.type==LIST) || (a.type==LIST&&b.type==MATRIX)) {
            c.type = MATRIX;

            if(a.type != MATRIX)
                a.matValue = a.listValue;
            if(b.type != MATRIX)
                b.matValue = b.listValue;

            if(op == "+")
                c.matValue = a.matValue + b.matValue;
            else if(op == "-")
                c.matValue = a.matValue - b.matValue;
            else if(op == "*")
                c.matValue = a.matValue * b.matValue;
            else if(op == "/") 
                throw "Invalid divide for Matrix";
        }
        else if(a.type == LIST && b.type == LIST) {
            c.type = LIST;

            if(a.listValue.size() != b.listValue.size())
                throw "List size not match.";

            c.listValue.resize(a.listValue.size());
            for(int k=0; k<a.listValue.size(); k++) {
                if(op == "+")
                    c.listValue[k] = a.listValue[k] + b.listValue[k];
                else if(op == "-")
                    c.listValue[k] = a.listValue[k] - b.listValue[k];
                else if(op == "*")
                    c.listValue[k] = a.listValue[k] * b.listValue[k];
                else if(op == "/")
                    c.listValue[k] = a.listValue[k] / b.listValue[k];
            }
        }
        else
            throw "Not supported now.";

        return c;
    }
}

#endif