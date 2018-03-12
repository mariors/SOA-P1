#include "loader.h"
#include "expropiative.h"


int main() 
{
    struct Property property;
    initProperty(&property);

    run_expropiative(&property);
 
    return 0;
}