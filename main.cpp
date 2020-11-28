#include <iostream>
#include <string.h>
#include "driver.h"

using namespace rtex;


int main(int argc, char const *argv[]) {
    Driver driver;

    for(int i=1; i<argc; i++) {
        if(strcmp(argv[i], "-d") == 0) {
            driver.setDebug(true);
        }
    }

    driver.parse();
    return 0;
}
