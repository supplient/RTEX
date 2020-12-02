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
    class LeftValue;
    class RightValue;
    class LoopCondition;
    

    //
    // Type define
    //

    typedef unordered_map<string, Var> SymbolTable;

    typedef function<void(SymbolTable)> Procedure;
    typedef function<Procedure(Procedure)> FuncForFunc;
    typedef function<LeftValue(SymbolTable)> LeftValueFunc;
    typedef function<RightValue(SymbolTable)> RightValueFunc;
    typedef function<bool(SymbolTable)> BoolFunc;
    typedef function<LoopCondition(SymbolTable)> LoopConditionFunc;

    typedef pair<RightValueFunc, BoolFunc> IfPair;

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
        static Type str2type(const string& s) {
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

    struct LeftValue {
        enum Type {
            VAR,
            MATRIX_ELE
        };

        Type type;
        string varName;
        vector<RightValueFunc> dimFuncs;
    };

    struct LoopCondition {
        Integer start;
        Integer step;
        Integer end;
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