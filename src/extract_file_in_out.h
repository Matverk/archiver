#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

#define WRITE_BUF_STEP 1024*50 // начальный размер и шаг увеличения буфера в байтах

/// @brief Читает таблицу кодов из сжатого файла
/// @param fin Входной сжатый `arh` файл
/// @param simbols Выходной массив символов
/// @return Количество записей (уникальных символов) 
int read_code_table(FILE* fin, symbol* simbols);

/// @brief Декодирует `fin` в `fout` по таблице `simbols`
/// @param fin Сжатый `arh` файл
/// @param fout Извлечённый файл
/// @param simbols Массив символов
/// @param uniqk Количество записей (уникальных символов)
/// @return -1, если ошибка памяти, иначе 0
int extract_from_file(FILE* fin, FILE* fout, symbol* simbols, int uniqk);