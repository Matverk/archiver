#include <stdio.h>
#include "types.h"
#include "math_func.h"

void set_information(symbol* simbols, int* k, int* kk, int* fsize_2) {
    for (int i = 0; i < *k; i++) {    
        printf("N= %d\tCode= %d\tFreq= %f\tNum=%f\tSymb=%c\n",i, simbols[i].ch, simbols[i].freq, simbols[i].freq*(*kk), simbols[i].ch);
    }
    printf("\nLetters = %d\n", *kk);
    printf("Entropy = %f\n", entropy_count(simbols, k));
    printf("Bit cost = %f\n", bcost(fsize_2, *kk));
}
