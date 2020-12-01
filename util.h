#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

using namespace std;

class Util {
public:
    static string join(vector<string> list, string sep, string head="") {
        string res = "";
        for(int i=0; i<list.size(); i++) {
            res += head + list[i];
            if(i != list.size()-1)
                res += sep;
        }
        return res;
    }
};

#endif //UTIL_H