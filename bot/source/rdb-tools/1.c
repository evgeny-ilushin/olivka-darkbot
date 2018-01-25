#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


int rnd(int max)
{
    return (int)(rand()/(RAND_MAX + 1.0)*max) + 1;
}

int main(char *a, char **b)
{
    srand(time(0));

    printf("%d\n", rnd(3));
    
    return 0;
}
