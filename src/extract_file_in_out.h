#pragma once
#include <stdio.h>
#include "types.h"

/// @brief Читает таблицу кодов из сжатого файла
/// @param fin Входной сжатый `arh` файл
/// @param simbols Выходной массив символов
/// @return Количество записей (уникальных символов) 
int read_code_table(FILE* fin, symbol* simbols);


void extract_from_file(FILE* fin, symbol* simbols, int uniqk);