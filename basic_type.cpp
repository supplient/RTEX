#include "basic_type.h"

using namespace rtex;

Matrix operator*(Real k, const Matrix& m) {
    Matrix res(m.GetN(), m.GetM());
    for(auto pair: m) {
        int i = pair.first;
        int j = pair.second;
        res[i][j] = k * m[i][j];
    }
    return res;
}