#include <iostream>
#include <string.h>
#include "driver.h"
#include "filter.h"

using namespace rtex;

bool debug = false;

void driverTest() {
    Driver driver(cin, cout, cerr);
    if(debug)
        driver.setDebug(true);
    driver.parse();
}


int main(int argc, char const *argv[]) {
    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-d") == 0) {
            debug = true;
        }
    }
    // driverTest();

    Filter filter(cin, cout, cerr);
    filter.yylex();

    return 0;
}
