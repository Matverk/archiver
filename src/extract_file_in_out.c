#include "extract_file_in_out.h"

/// @brief Получение бита на `i`-том месте байта 
/// @param code1 Байт
/// @param ind Индекс (номер от 1 до 8 включительно)
/// @return Символ по номеру бита
char get_bit(union code* code1, int ind) {
    unsigned bit = 0;
    switch (ind) {
    case 1:
        bit = code1->byte.b1;
        break;
    case 2:
        bit = code1->byte.b2;
        break;
    case 3:
        bit = code1->byte.b3;
        break;
    case 4:
        bit = code1->byte.b4;
        break;
    case 5:
        bit = code1->byte.b5;
        break;
    case 6:
        bit = code1->byte.b6;
        break;
    case 7:
        bit = code1->byte.b7;
        break;
    case 8:
        bit = code1->byte.b8;
        break;
    }
    return bit ? '1' : '0';
}

int read_code_table(FILE* fin, symbol* simbols) {
    int uniqk = getc(fin) + 1;
    union code cd1;
    cd1.sym_to_write = '\0';
    for (int i = 0; i < uniqk; ++i) {
        simbols[i].ch = (unsigned char)getc(fin);
        sprintf(simbols[i].code, "");
        while (1) {
            cd1.sym_to_write = getc(fin);
            if (!(cd1.byte.b1 && cd1.byte.b2)) {    // полное считывание 6-и бит кода (случай 0 1 невозможен)
                for (int j = 3; j <= 8; ++j) sprintf(simbols[i].code, "%s%c", simbols[i].code, get_bit(&cd1, j));
            }
            if (cd1.byte.b1) {      // конечный байт
                if (cd1.byte.b2) {  // умная граница
                    char lastch = get_bit(&cd1, 8);
                    short lsti;     // будет последний индекс бита с кодом
                    for (int b = 7; b >= 3; --b) {
                        if (get_bit(&cd1, b) != lastch) {
                            lsti = b;
                            break;
                        }
                    }
                    for (int j = 3; j <= lsti; ++j) sprintf(simbols[i].code, "%s%c", simbols[i].code, get_bit(&cd1, j));
                }
                break;
            }
        }
    }
    return uniqk;
}