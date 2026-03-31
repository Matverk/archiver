#include "math_func.h"

float entropy_count(symbol* simbols[], int k) {
    float sum = 0;
    for (short i = 0; i < k; ++i) {
        float pk = simbols[i]->freq;
        sum += pk * log2((double)pk);
    }
    return -sum;
}

float bit_costs(int fsize_2, int allk) {
    return (float)fsize_2 / allk;
}
