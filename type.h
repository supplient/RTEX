#ifndef TYPE_H
#define TYPE_H

#include <unordered_map>
#include <memory>
#include <vector>
#include "matrix.h"

namespace rtex {
    using namespace std;
    
    //
    // Type define
    //

    template<class T, class U=string>
    struct Bag {
        T v;
        U s;
    };

    struct Var {
        enum {
            MATRIX,
            REAL,
            INTEGER
        } type;
        int id;
    };

    typedef unordered_map<string, Var> SymbolTable;
}

#endif //TYPE_H