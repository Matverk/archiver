#include <math.h>
#include "math_func.h"

float entropy_count(symbol* simbols, int* k) {
    float sum = 0;
    for (symbol* spt = simbols; spt < simbols + *k; ++spt) {
        float pk = spt->freq;
        sum += pk * log2((double)pk);
    }
    return -sum;
}

double bit_costs(int* fsize_2, int allk) {
    return (double)*fsize_2 / (double)allk;
}
