#ifndef DRIVER_HH
#define DRIVER_HH

#include <iostream>

#include "type.h"
#include "scanner.h"
#include "parser.tab.h"
 
namespace rtex {

    using namespace std;

    class Driver {
        friend class Scanner;
        friend class Parser;

    private:
        Parser m_parser;
        Scanner m_scanner;

    public:
        Driver ();
        int parse();
        void setDebug(bool flag);
        virtual ~Driver ();

        ostream& out = cout;
        bool outputMarkdown = false;

        vector<Matrix> matTbl;
        vector<Real> realTbl;
        vector<Integer> intTbl;
    };

} /*rtex */
#endif