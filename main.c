#include <stdio.h>

#include "arctan_func.h"

int main(int argc, char** argv){
    int iter = 1;
    long double val = 0.0;
    while(1){
        val +=  arctan_aproximation(1,iter++);
        printf("%Lf\n", val);
    }
    return 0;
}
