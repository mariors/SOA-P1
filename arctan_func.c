#include "arctan_func.h"

int L = 500;

long double arctan_aproximation(int x, int iter){
    return 2 * (4*L * (x/ (pow(2*iter-1,2)*pow(x,2)+(4*pow(L,2)))));
}