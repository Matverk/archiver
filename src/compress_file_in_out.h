#ifndef FILE_H3
#define FILE_H3
#include <stdio.h>
#include <string.h>
#include "types.h"

/// @brief Чтение и группировка символов
/// @param fp входной бинарный файл
/// @param simbols выходной массив уникальных символов
/// @param kolvo выходной массив частот символов
/// @param allk количество прочитанных символов
/// @param uniqk количество уникальных символов
void reading_from_file(FILE* fp, symbol* simbols, int* kolvo, int* allk, int* uniqk);

/// @brief Записывает подготовленный код (0,1 в текстовом виде) в сжатый файл `fp3`
/// @param fin входной файл
/// @param fp3 выходной сжатый файл
/// @param simbols массив уникальных символов
/// @param uniqk количество уникальных символов
/// @param fsize2 выходное количество 0 и 1 в сжатом файле
void compress_to_file(FILE* fin, FILE* fp3, symbol* simbols, int uniqk, int* fsize2);

/// @brief Записывает сжатый файл из таблицы кодов `simbols`. Быстрее чем `compress_to_file()`
/// @param fin исходный файл
/// @param fp3 выходной сжатый файл
/// @param simbols массив уникальных символов
/// @param uniqk количество уникальных символов
/// @param fsize2 количество бит в сжатом файле (без учёта таблицы кодов)
void compress_to_file_simb(FILE* fin, FILE* fp3, symbol* simbols, int uniqk, int* fsize2);

/// @brief Записывает таблицу новых кодов в файл `fp3`
/// @param fp3 сжатый файл
/// @param simbols массив уникальных символов
/// @param uniqk количество уникальных символов
/// @param allk количество символов в исходном файле
void write_code_table(FILE* fp3, symbol* simbols, int uniqk, int allk);
#endif