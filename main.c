#include <stdio.h>
#include <setjmp.h>
#include <math.h>
#include <unistd.h>


long double function(int iter){
    return pow(-1,iter) * pow(1,(2*iter+1))/(2*iter + 1);
}

int main(int argc, char** argv){
    int iter = 0;
    long double val = 0.0;
    while(1){
        val +=  function(iter++);
        printf("%Lf\n",4 * val);
        sleep(1);
    }
    return 0;
}
