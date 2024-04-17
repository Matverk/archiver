#include <stdio.h>
#include <string.h>
#include "file_In_out.h"

void reading_from_file(FILE* fp, symbol* simbols, int* kolvo, int* allk, int* uniqk) {
    unsigned char tmpc;
    while (1) {
        tmpc = fgetc(fp);
        if (feof(fp)) break;
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
            code1.byte.b1 = ch - '0';    // 48 - код символа 0
            break;
        case 2:
            code1.byte.b2 = ch - '0';
            break;
        case 3:
            code1.byte.b3 = ch - '0';
            break;
        case 4:
            code1.byte.b4 = ch - '0';
            break;
        case 5:
            code1.byte.b5 = ch - '0';
            break;
        case 6:
            code1.byte.b6 = ch - '0';
            break;
        case 7:
            code1.byte.b7 = ch - '0';
            break;
        case 8:
            code1.byte.b8 = ch - '0';
            break;
        }
        ++i;
    }
    fputc(code1.sym_to_write, fp3);
}

void writing_to_file_simb(FILE* fin, FILE* fp3, symbol* simbols, int* uniqk, int* fsize2) {
    char ch;
    union code code1;
    int i = 1;
    *fsize2 = 0;
    while (1) {
        ch = fgetc(fin);
        if (feof(fin)) break;
        for (int c = 0; c < *uniqk; ++c) {
            if (simbols[c].ch == (unsigned char)ch) {
                for (int j = 0; j < strlen(simbols[c].code); ++j) {
                    if (i > 8) {
                        i = 1;
                        fputc(code1.sym_to_write, fp3);
                        code1.sym_to_write = '\0';
                    }
                    switch (i) {
                    case 1:
                        code1.byte.b1 = simbols[c].code[j] - '0';
                        break;
                    case 2:
                        code1.byte.b2 = simbols[c].code[j] - '0';
                        break;
                    case 3:
                        code1.byte.b3 = simbols[c].code[j] - '0';
                        break;
                    case 4:
                        code1.byte.b4 = simbols[c].code[j] - '0';
                        break;
                    case 5:
                        code1.byte.b5 = simbols[c].code[j] - '0';
                        break;
                    case 6:
                        code1.byte.b6 = simbols[c].code[j] - '0';
                        break;
                    case 7:
                        code1.byte.b7 = simbols[c].code[j] - '0';
                        break;
                    case 8:
                        code1.byte.b8 = simbols[c].code[j] - '0';
                        break;
                    }
                    ++i;
                    ++(*fsize2);
                }
            }
        }
    }
    fputc(code1.sym_to_write, fp3);
}