#include <stdio.h>
#include "math_func.h"
#include "information.h"

void show_information(symbol* simbols, int uniqk, int allk, int fsize_2) {
    for (int i = 0; i < uniqk; i++)
        printf("N: %i\tCode: %d\tFreq: %f\tQuant: %.0f\tSymb: \"%c\"\tCompressed: %s\n",
            i, simbols[i].ch, simbols[i].freq, simbols[i].freq * allk, simbols[i].ch, simbols[i].code);
    printf("\nLetters: %d\n", allk);
    printf("Entropy: %f\n", entropy_count(simbols, &uniqk));
    printf("Bit costs: %f\n", bit_costs(&fsize_2, allk));
}
