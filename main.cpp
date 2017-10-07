#include "matrix.hpp"
using namespace matrix;
int test();

int main(int argc, const char *argv[]) {
    using mat = matrix::Matrix<int>;
    using vec = matrix::Vector<int>;
    test();
    mat c{{1,2,3},{1,2,3},{1,2,3}};
    vec a{1,2,3};
    vec b{1,1,1};
    // mat x = b.dot(b.transpose());
    cout << a.dot(b.transpose());
    // cout << (a+1);
    // x.print();
    // cout << x.shape()[0] << endl;
    cout << "-------------:" << matrix::cnt << endl;
    return 0;
}