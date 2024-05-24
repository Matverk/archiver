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

void extract_from_file(FILE* fin, FILE* fout, symbol* simbols, int uniqk) {
    size_t text_start = ftell(fin); // первый байт сжатого текста
    fseek(fin, 0, SEEK_END);
    size_t text_len = ftell(fin) - text_start;  // длина сжатого текста в байтах
    fseek(fin, text_start, SEEK_SET);
    char* buf = (char*)malloc(text_len + 1);    // байты как в файле
    buf[text_len] = '\0';
    fread(buf, 1, text_len, fin);   // читаем до конца файла

    size_t text_bitlen = text_len * 8;  // битов в тексте
    char* buf_str = (char*)malloc(text_bitlen + 1); // бит файла -->> байт (char)
    for (size_t i = 0; i < text_len; ++i) { // преобразование в массив, где каждый бит - это '0' или '1'
        union code cd1;
        cd1.sym_to_write = buf[i];
        for (int j = 1; j <= 8; ++j) buf_str[i * 8 + j - 1] = get_bit(&cd1, j);
    }
    buf_str[text_bitlen] = '\0';

    size_t cur_bit = 0; // текущий первый бит
    while (cur_bit < text_bitlen) {
        for (int i = 0; i < uniqk; ++i) {
            int codelen = strlen(simbols[i].code);
            size_t boundbit = cur_bit + codelen;
            if (boundbit > text_bitlen) codelen -= boundbit - text_bitlen;  // обработка выхода за границу
            if (!strncmp(buf_str + cur_bit, simbols[i].code, codelen)) {
                putc(simbols[i].ch, fout);
                cur_bit += codelen;
                break;
            }
        }
    }
    free(buf);
    free(buf_str);
}