#ifndef FILE_H3
#define FILE_H3
#include "types.h"

/// @brief Чтение и группировка символов
/// @param fp входной бинарный файл
/// @param simbols выходной массив уникальных символов
/// @param kolvo выходной массив частот символов
/// @param allk количество прочитанных символов
/// @param uniqk количество уникальных символов
void reading_from_file(FILE* fp, symbol* simbols, int* kolvo, int* allk, int* uniqk);

/// @brief Записывает подготовленный код из `fp2` в сжатый файл `fp3`
/// @param fp2 подготовленный файл из 0 и 1
/// @param fp3 выходной сжатый файл
/// @param fsize2 выходное количество 0 и 1 в сжатом файле
void writing_to_file(FILE* fp2, FILE* fp3, int* fsize2);
#endif
