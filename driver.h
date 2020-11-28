#ifndef DRIVER_HH
#define DRIVER_HH
#include <iostream>
#include "scanner.h"
#include "parser.tab.h"
 
namespace rtex {
    class Driver {
        friend class Scanner;
        friend class Parser;
    private:
        /* data */
        Parser m_parser;
        Scanner m_scanner;
    public:
        Driver ();
        int parse();
        void setDebug(bool flag);
        virtual ~Driver ();
    };
} /*rtex */
#endif