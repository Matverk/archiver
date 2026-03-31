#ifndef FILE_H4
#define FILE_H4
#include <stdio.h>
#include "types.h"
#include "math_func.h"

/// @brief 
/// @param simbols массив символов (указатели)
/// @param uniqk количество уникальных символов
/// @param allk количество прочитанных символов
/// @param fsize_2 кол-во бит в сжатом файле
void show_information(symbol* simbols[], int uniqk, int allk, int fsize_2);
#endif