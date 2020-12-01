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
            return [&typeName, &varName, dimFuncs, this](SymbolTable lSymTbl) {
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

        RightValueFunc solve_right_exp_op(RightValueFunc af, RightValueFunc bf, string& op) {
            return [af, bf, &op, this](SymbolTable lSymTbl) {
                RightValue a = af(lSymTbl);
                RightValue b = bf(lSymTbl);
                RightValue c;

                if(a.type == b.type)
                    c.type = a.type;
                else {
                    if(a.type != RightValue::Type::REAL)
                        a.realValue = a.intValue;
                    if(b.type != RightValue::Type::REAL)
                        b.realValue = b.intValue;
                    c.type = RightValue::Type::REAL;
                }

                if(c.type == RightValue::Type::INTEGER) {
                    if(op == "+")
                        c.intValue = a.intValue + b.intValue;
                    else if(op == "-")
                        c.intValue = a.intValue - b.intValue;
                    else if(op == "*")
                        c.intValue = a.intValue * b.intValue;
                    else if(op == "/")
                        c.intValue = a.intValue / b.intValue;
                }
                else {
                    if(op == "+")
                        c.realValue = a.realValue + b.realValue;
                    else if(op == "-")
                        c.realValue = a.realValue - b.realValue;
                    else if(op == "*")
                        c.realValue = a.realValue * b.realValue;
                    else if(op == "/")
                        c.realValue = a.realValue / b.realValue;
                }

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
                    else
                        c.realValue = cos(a.realValue);
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


    };

} /*rtex */
#endif