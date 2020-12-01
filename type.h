#ifndef TYPE_H
#define TYPE_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>

#include "basic_type.h"

namespace rtex {
    using namespace std;

    //
    // Forward Declaration
    //

    class Var;
    class RightValue;
    

    //
    // Type define
    //

    typedef unordered_map<string, Var> SymbolTable;

    typedef function<void(SymbolTable)> Procedure;
    typedef function<Procedure(Procedure)> FuncForFunc;
    typedef function<RightValue(SymbolTable)> RightValueFunc;

    template<class T, class U=string>
    struct Bag {
        T v;
        U s;
    };

    struct Var {
        enum Type {
            MATRIX,
            REAL,
            INTEGER
        };
        static Type str2type(string& s) {
            if(s=="Matrix")
                return MATRIX;
            else if(s=="Real")
                return REAL;
            else if(s=="Integer")
                return INTEGER;
            else
                throw "ERROR";
        }

        Type type;
        int id;
    };

    struct RightValue {
        enum Type {
            REAL,
            INTEGER
        };

        Type type;
        Integer intValue;
        Real realValue;
    };

    struct Phase {
        enum Type {
            PROCEDURE,
            FUNC_FOR_FUNC,
        };

        Type type;
        Procedure prod;
        FuncForFunc fff;
    };
}

#endif //TYPE_H