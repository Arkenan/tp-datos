#include <iostream>
#include <armadillo>
using namespace std;
using namespace arma;

#define SDG_N 6

int main(){
    mat A(17,2);
    A.fill(1.0);
    for (int i = 0; i < A.n_rows; i++){
        A.row(i) = A.row(i)*i;
    }
    A.print("A:");
    mat B;
    // Tomo matrices de tres filas.
    int its = A.n_rows/SDG_N;
    for (int i = 0; i < its; i++){
        B = A.rows(SDG_N*i, SDG_N*(i+1)-1);
        cout << "iteración " << i << " ";
        B.print("B:");
    }
    // Las que quedan.
    B = A.rows(its*SDG_N, A.n_rows - 1);
    B.print("Último B: ");
    return 0;
}
