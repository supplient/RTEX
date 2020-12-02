#ifndef DRIVER_HH
#define DRIVER_HH

#include <iostream>

#include "type.h"
#include "scanner.h"
#include "parser.tab.h"
 
namespace rtex {

    using namespace std;

    template<class T, class U>
    bool solve_bool_exp_Compare_Helper(T a, U b, string op) {
        if(op == ">")
            return a > b;
        else if(op == "<")
            return a < b;
        else if(op == "==")
            return a == b;
        else if(op == "!=")
            return a != b;
        else if(op == ">=")
            return a >= b;
        else if(op == "<=")
            return a <= b;
        else
            throw "Invalid bool opeartor.";
    }

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

        void printVar(const Var& v) {
            switch (v.type)
            {
            case Var::Type::MATRIX:
                out << matTbl[v.id];
                break;
            case Var::Type::INTEGER:
                out << intTbl[v.id];
                break;
            case Var::Type::REAL:
                out << realTbl[v.id];
                break;
            default:
                break;
            }
        }

        ostream& out = cout;
        bool outputMarkdown = false;

        vector<Matrix> matTbl;
        vector<Real> realTbl;
        vector<Integer> intTbl;

        SymbolTable gSymTbl;

        Var searchVar(string& varName, SymbolTable& lSymTbl) {
            if(lSymTbl.find(varName) != lSymTbl.end())
                return lSymTbl[varName];
            else if(gSymTbl.find(varName) != gSymTbl.end())
                return gSymTbl[varName];
            throw "Undefined identifier";
        }

        void solve_statement(vector<Phase> phases) {
            vector<Procedure> prods;
            int startI = 0;
            int endI = -1;
            Procedure tmp;
            for(Phase& phase: phases) {
                switch (phase.type)
                {
                case Phase::Type::PROCEDURE:
                    prods.push_back(phase.prod);
                    break;
                case Phase::Type::FUNC_FOR_FUNC:
                    endI = prods.size();
                    tmp = phase.fff(
                        [&prods, startI, endI](SymbolTable lSymTbl) {
                            for(int i=startI; i<endI; i++)
                                prods[i](lSymTbl);
                        }
                    );
                    startI = prods.size();
                    prods.push_back(tmp);
                    break;
                default:
                    break;
                }
            }
            for(int i=startI; i<prods.size(); i++)
                prods[i]({});
        }

        Procedure solve_type_phase(string& typeName, string& varName, vector<RightValueFunc> dimFuncs={}) {
            return [typeName, varName, dimFuncs, this](SymbolTable lSymTbl) {
                // Determine type
                Var::Type type = Var::str2type(typeName);

                if(gSymTbl.find(varName) != gSymTbl.end())            
                    throw "Error";

                // Cal dims
                vector<int> dims;
                for(RightValueFunc f: dimFuncs) {
                    RightValue v = f(lSymTbl);
                    if(v.type != RightValue::Type::INTEGER)
                        throw "Error";
                    dims.push_back(v.intValue);
                }
                
                // Determine id & Fill var table
                int id;
                switch (type)
                {
                case Var::Type::REAL:
                    id = realTbl.size();
                    realTbl.push_back(0);
                    break;
                case Var::Type::INTEGER:
                    id = intTbl.size();
                    intTbl.push_back(0);
                    break;
                case Var::Type::MATRIX:
                    if(dims.size() < 1 || dims.size() > 2)
                        throw "Error";
                    id = matTbl.size();
                    if(dims.size()==1)
                        matTbl.push_back({dims[0], dims[0]});
                    else
                        matTbl.push_back({dims[0], dims[1]});
                    break;
                default:
                    break;
                }

                // Fill symbol table
                gSymTbl[varName] = {type, id};
            };
        }

        Procedure solve_assign_phase(LeftValueFunc leftValueFunc, RightValueFunc rightValueFunc) {
            return [leftValueFunc, rightValueFunc, this](SymbolTable lSymTbl) {
                LeftValue lv = leftValueFunc(lSymTbl);
                RightValue rv = rightValueFunc(lSymTbl);

                if(lv.type == LeftValue::Type::VAR) {
                    Var var = searchVar(lv.varName, lSymTbl);
                    if(var.type == Var::Type::REAL) {
                        if(rv.type == RightValue::Type::INTEGER)
                            realTbl[var.id] = rv.intValue;
                        else if(rv.type == RightValue::Type::REAL)
                            realTbl[var.id] = rv.realValue;
                        else
                            throw "Type not match.";
                    }
                    else if(var.type == Var::Type::INTEGER) {
                        if(rv.type == RightValue::Type::INTEGER)
                            intTbl[var.id] = rv.intValue;
                        else if(rv.type == RightValue::Type::REAL)
                            intTbl[var.id] = Integer(rv.realValue);
                        else
                            throw "Type not match.";
                        
                    }
                    else if(var.type == Var::Type::MATRIX) {
                        if(rv.type == RightValue::Type::MATRIX)
                            matTbl[var.id] = rv.matValue;
                        else if(rv.type == RightValue::Type::LIST)
                            matTbl[var.id] = rv.listValue;
                        else
                            throw "left-exp is Matrix type, so right-exp must be Matrix or List";
                    }
                }
                else {
                    Var var = searchVar(lv.varName, lSymTbl);
                    if(var.type != Var::Type::MATRIX)
                        throw "Only Matrix can be referred by dims";
                    Matrix& m = matTbl[var.id];
                    
                    vector<Integer> dims;
                    for(auto f: lv.dimFuncs) {
                        RightValue rv = f(lSymTbl);
                        if(rv.type != RightValue::INTEGER)
                            throw "Dim must be Integer.";
                        dims.push_back(rv.intValue);
                    }
                    if(dims.size() != 2)
                        throw "Matrix must be two-dims";

                    if(rv.type == RightValue::Type::INTEGER)
                        m[dims[0]][dims[1]] = rv.intValue;
                    else if(rv.type == RightValue::Type::REAL)
                        m[dims[0]][dims[1]] = rv.realValue;
                    else
                        throw "Type not match.";
                }
            };
        }

        FuncForFunc solve_where_phase(string varName, LoopConditionFunc loopf) {
            return [varName, loopf, this](Procedure proc) {
                return [varName, loopf, proc, this](SymbolTable lSymTbl) {
                    // Cal loop condition
                    LoopCondition loopCond = loopf(lSymTbl);

                    // Create local var
                    Var var;
                    var.type = Var::Type::INTEGER;
                    var.id = intTbl.size();
                    intTbl.push_back(0);

                    // Fill local symbol table
                    lSymTbl[varName] = var;

                    // Do loop
                    for(Integer i=loopCond.start; ; i+=loopCond.step) {
                        intTbl[var.id] = i;
                        proc(lSymTbl);

                        if(i == loopCond.end)
                            break;
                    }
                };
            };
        }

        LoopConditionFunc solve_ellipsis_exp(RightValueFunc startf, RightValueFunc secf, RightValueFunc endf) {
            return [startf, secf, endf](SymbolTable lSymTbl) {
                RightValue start = startf(lSymTbl);
                RightValue sec = secf(lSymTbl);
                RightValue end = endf(lSymTbl);

                if(
                    start.type!=RightValue::Type::INTEGER
                    || sec.type!=RightValue::Type::INTEGER
                    || end.type!=RightValue::Type::INTEGER
                    )
                    throw "Can only use Integer for ellipsis-exp";
                
                LoopCondition res;
                res.start = start.intValue;
                res.step = sec.intValue - start.intValue;
                res.end = end.intValue;
                return res;
            };
        }

        LeftValueFunc solve_left_exp_var(string& varName) {
            return [varName](SymbolTable lSymTbl) {
                LeftValue res;
                res.type = LeftValue::Type::VAR;
                res.varName = varName;
                return res;
            };
        }

        LeftValueFunc solve_left_exp_mat(string& varName, vector<RightValueFunc> dimFuncs) {
            return [varName, dimFuncs](SymbolTable lSymTbl) {
                LeftValue res;
                res.type = LeftValue::Type::MATRIX_ELE;
                res.varName = varName;
                res.dimFuncs = dimFuncs;
                return res;
            };
        }

        RightValueFunc solve_right_exp_op(RightValueFunc af, RightValueFunc bf, string& op) {
            return [af, bf, op, this](SymbolTable lSymTbl) {
                RightValue a = af(lSymTbl);
                RightValue b = bf(lSymTbl);
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
            };
        }

        RightValueFunc solve_right_exp_func(RightValueFunc af, string funcName) {
            return [af, funcName, this](SymbolTable lSymTbl) {
                RightValue a = af(lSymTbl);
                RightValue c;

                if(funcName == "cos") {
                    c.type = RightValue::Type::REAL;
                    if(a.type == RightValue::Type::INTEGER)
                        c.realValue = cos(a.intValue);
                    else if(a.type == RightValue::Type::REAL)
                        c.realValue = cos(a.realValue);
                    else
                        throw "Invalid type for cos";
                }
                return c;
            };
        }

        RightValueFunc solve_right_exp_int(Integer d) {
            return [d, this](SymbolTable lSymTbl) ->RightValue{
                return {RightValue::Type::INTEGER, d, 0};
            };
        }

        RightValueFunc solve_right_exp_real(Real d) {
            return [d, this](SymbolTable lSymTbl) ->RightValue {
                return {RightValue::Type::REAL, 0, d};
            };
        }

        RightValueFunc solve_right_exp_from_left_exp(LeftValueFunc leftExpFunc) {
            return [leftExpFunc, this](SymbolTable lSymTbl) {
                LeftValue v = leftExpFunc(lSymTbl);
                RightValue res;

                if(v.type == LeftValue::Type::VAR) {
                    Var var = searchVar(v.varName, lSymTbl);
                    switch (var.type)
                    {
                    case Var::Type::REAL:
                        res.type = RightValue::Type::REAL;
                        res.realValue = realTbl[var.id];
                        break;
                    case Var::Type::INTEGER:
                        res.type = RightValue::Type::INTEGER;
                        res.intValue = intTbl[var.id];
                        break;
                    case Var::Type::MATRIX:
                        res.type = RightValue::Type::MATRIX;
                        res.matValue = matTbl[var.id];
                    default:
                        throw "Error";
                        break;
                    }
                }
                else {
                    Var var = searchVar(v.varName, lSymTbl);
                    if(var.type != Var::Type::MATRIX)
                        throw "Can only refer dims for matrix";
                    Matrix& m = matTbl[var.id];

                    vector<Integer> dims;
                    for(auto f: v.dimFuncs) {
                        RightValue rv = f(lSymTbl);
                        if(rv.type != RightValue::INTEGER)
                            throw "Dim must be Integer.";
                        dims.push_back(rv.intValue);
                    }
                    if(dims.size() != 2)
                        throw "Matrix must be two-dims";

                    res.type = RightValue::Type::REAL;
                    res.realValue = m[dims[0]][dims[1]];
                }
                return res;
            };
        }

        RightValueFunc solve_list_exp_list(vector<RightValueFunc> fs) {
            return [fs](SymbolTable lSymTbl) {
                RightValue res;
                res.type = RightValue::Type::LIST;
                for(auto f: fs) {
                    RightValue v = f(lSymTbl);
                    switch (v.type)
                    {
                    case RightValue::Type::INTEGER:
                        v.realValue = v.intValue;
                    case RightValue::Type::REAL:
                        res.listValue.push_back(v.realValue);
                        break;
                    default:
                        throw "List cannot contain list or matrix.";
                        break;
                    }
                }
                return res;
            };
        }

        RightValueFunc solve_list_exp_mat(Integer n, Integer m, vector<RightValueFunc> fs) {
            return [n, m, fs](SymbolTable lSymTbl) {
                RightValue res;
                res.type = RightValue::Type::MATRIX;
                res.matValue.ResetSize(n, m);

                int k = 0;
                for(auto f: fs) {
                    int i = k / m;
                    int j = k % m;
                    k++;

                    RightValue v = f(lSymTbl);
                    switch (v.type)
                    {
                    case RightValue::Type::INTEGER:
                        v.realValue = v.intValue;
                    case RightValue::Type::REAL:
                        res.matValue[i][j] = v.realValue;
                        break;
                    default:
                        throw "Matrix cannot contain list or matrix.";
                        break;
                    }
                }
                return res;
            };
        }

        RightValueFunc solve_if_exp(vector<IfPair>& ifPairs) {
            return [ifPairs](SymbolTable lSymTbl)->RightValue {
                for(const IfPair& p: ifPairs) {
                    const BoolFunc& bf = p.second;
                    if(bf(lSymTbl))
                        return p.first(lSymTbl);
                }
                throw "if-exp has no valid value, add else-exp-phase please.";
            };
        }

        IfPair solve_if_exp_phase_if(RightValueFunc rightValueFunc, BoolFunc boolFunc) {
            IfPair res;
            res.first = rightValueFunc;
            res.second = boolFunc;
            return res;
        }

        IfPair solve_if_exp_phase_else(RightValueFunc rightValueFunc) {
            IfPair res;
            res.first = rightValueFunc;
            res.second = [](SymbolTable lSymTbl) { return true; };
            return res;
        }

        BoolFunc solve_bool_exp(RightValueFunc af, RightValueFunc bf, string op) {
            return [af, bf, op](SymbolTable lSymTbl) {
                RightValue a = af(lSymTbl);
                RightValue b = bf(lSymTbl);

                auto REAL = RightValue::Type::REAL;
                auto INTEGER = RightValue::Type::INTEGER;

                if(a.type==REAL && b.type==REAL)
                    return solve_bool_exp_Compare_Helper(a.realValue, b.realValue, op);
                else if(a.type==INTEGER && b.type==REAL)
                    return solve_bool_exp_Compare_Helper(a.intValue, b.realValue, op);
                else if(a.type==REAL && b.type==INTEGER)
                    return solve_bool_exp_Compare_Helper(a.realValue, b.intValue, op);
                else if(a.type==INTEGER && b.type==INTEGER)
                    return solve_bool_exp_Compare_Helper(a.intValue, b.intValue, op);
                else
                    throw "Cannot compare between Matrix or List.";
            };
        }

    };

} /*rtex */
#endif