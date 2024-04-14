#include <stdio.h>
#include "types.h"
#include "file_In_out.h"

void reading_from_file(FILE* fp, symbol* simbols, int* kolvo, int* allk, int* uniqk) {
    unsigned char tmpc;
    while ((tmpc = fgetc(fp)) && !feof(fp)) {
        ++(*allk);
        int is_new = 1;
        int i;
        for (i = 0; i < *uniqk; ++i) {
            if (tmpc == simbols[i].ch) {
                is_new = 0;
                break;
            }
        }
        if (is_new) {
            symbol s;
            s.ch = tmpc;
            simbols[*uniqk] = s;
            kolvo[*uniqk] = 1;
            ++(*uniqk);
        }
        else ++(kolvo[i]);
    }
}

void writing_to_file(FILE* fp2, FILE* fp3, symbol* simbols, int* kolvo, int* kk, int* k, int* fsize2) {

    // :)
}

