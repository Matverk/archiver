#include <stdio.h>
#include <string.h>
#include "compress_file_in_out.h"

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
/// @brief Сборка байта побитово в `code1` для записи в сжатый файл. Производится конвертация из `char` в бит.
/// @param i номер бита для вставки - 1...8
/// @param ch '0' или '1'
void insert_bit(union code* code1, int i, char ch) {
    switch (i) {
    case 1:
        code1->byte.b1 = ch - '0';
        break;
    case 2:
        code1->byte.b2 = ch - '0';
        break;
    case 3:
        code1->byte.b3 = ch - '0';
        break;
    case 4:
        code1->byte.b4 = ch - '0';
        break;
    case 5:
        code1->byte.b5 = ch - '0';
        break;
    case 6:
        code1->byte.b6 = ch - '0';
        break;
    case 7:
        code1->byte.b7 = ch - '0';
        break;
    case 8:
        code1->byte.b8 = ch - '0';
        break;
    }
}

void compress_to_file(FILE* fin, FILE* fp3, symbol* simbols, int uniqk, int* fsize2) {
    unsigned int chh;
    FILE* fp2 = fopen("temp", "w");
    //в цикле читаем исходный файл, и записываем полученные в функциях коды в промежуточный файл
    while ((chh = fgetc(fin)) != EOF) {
        for (int i = 0; i < uniqk; i++)
            if (chh == simbols[i].ch) fputs(simbols[i].code, fp2);
    }
    fclose(fp2);
    fp2 = fopen("temp", "rb");
    char ch;
    int i = 1;
    union code code1;
    while ((ch = fgetc(fp2)) != EOF) {
        if (i > 8) {
            i = 1;
            fputc(code1.sym_to_write, fp3);
            code1.sym_to_write = '\0';
        }
        insert_bit(&code1, i, ch);
        ++i;
        ++(*fsize2);
    }
    fputc(code1.sym_to_write, fp3);
}

void compress_to_file_simb(FILE* fin, FILE* fp3, symbol* simbols, int uniqk, int* fsize2) {
    char ch;
    union code code1;
    int i = 1;
    *fsize2 = 0;
    while (1) {
        ch = fgetc(fin);    // для каждого символа в исходном файле пишем его новый код
        if (feof(fin)) break;
        for (int c = 0; c < uniqk; ++c) {
            if (simbols[c].ch == (unsigned char)ch) {
                for (int j = 0; j < strlen(simbols[c].code); ++j) {
                    if (i > 8) {
                        i = 1;
                        fputc(code1.sym_to_write, fp3);
                        code1.sym_to_write = '\0';
                    }
                    insert_bit(&code1, i, simbols[c].code[j]);
                    ++i;
                    ++(*fsize2);
                }
            }
        }
    }
    fputc(code1.sym_to_write, fp3);
}

void write_code_table(FILE* fp3, symbol* simbols, int uniqk) {
    putc((unsigned char)uniqk - 1, fp3);    // количество уник. символов
    for (int i = 0; i < uniqk; ++i) {
        char* code_str = simbols[i].code;
        int codelen = strlen(code_str);
        int leftlen = codelen;
        putc(simbols[i].ch, fp3);   // пишем исходный символ
        union code cd1;
        while (leftlen) {
            cd1.sym_to_write = '\0';
            cd1.byte.b1 = leftlen <= 6; // конец?
            cd1.byte.b2 = leftlen < 6;  // умная граница?
            if (cd1.byte.b1) {  // последний байт - конец
                int lsti = 9;   // будет последний записанный индекс с кодом
                for (int j = 3; j <= 8; ++j) {
                    if (j - 3 < leftlen) insert_bit(&cd1, j, code_str[codelen - leftlen + j - 3]);
                    else {
                        lsti = j;
                        break;
                    }
                }
                if (cd1.byte.b2) {  // умная граница
                    char symb_fill = (code_str[codelen - 1] == '0') ? '1' : '0';
                    for (int j = lsti; j <= 8; ++j) insert_bit(&cd1, j, symb_fill);
                }
                leftlen = 0;
            }
            else {
                for (int j = 3; j <= 8; ++j) insert_bit(&cd1, j, code_str[codelen - leftlen + j - 3]);
                leftlen -= 6;
            }
            putc(cd1.sym_to_write, fp3);
        }
    }
}