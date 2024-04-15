#include <stdio.h>
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
            sprintf(s.code, "");
            s.left = NULL;
            s.right = NULL;
            simbols[*uniqk] = s;
            kolvo[*uniqk] = 1;
            ++(*uniqk);
        }
        else ++(kolvo[i]);
    }
}

void writing_to_file(FILE* fp2, FILE* fp3, int* fsize2) {
    char ch;
    int i = 1;
    union code code1;
    while ((ch = fgetc(fp2)) != EOF) {
        ++(*fsize2);
        if (i > 8) {
            i = 1;
            fputc(code1.sym_to_write, fp3);
            code1.sym_to_write = '\0';
        }
        switch (i) {
        case 1:
            code1.byte.b1 = ch - 48;    // 48 - код символа 0
            break;
        case 2:
            code1.byte.b2 = ch - 48;
            break;
        case 3:
            code1.byte.b3 = ch - 48;
            break;
        case 4:
            code1.byte.b4 = ch - 48;
            break;
        case 5:
            code1.byte.b5 = ch - 48;
            break;
        case 6:
            code1.byte.b6 = ch - 48;
            break;
        case 7:
            code1.byte.b7 = ch - 48;
            break;
        case 8:
            code1.byte.b8 = ch - 48;
            break;
        default:
            break;
        }
        ++i;
    }
    fputc(code1.sym_to_write, fp3);
}