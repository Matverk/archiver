#ifndef FILE_H5
#define FILE_H5
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

/// @brief Рeкурсивная функция создания дерева Хафмана.
/// @param psym по указателю значение нового пришедшего элемента. В данном случае, самого нижнего.
/// @param k общее количество символов
/// @return указатель на новую структуру типа symbol или `NULL`, если ошибка памяти
symbol* makeTree(symbol* psym[], int k);

/// @brief Рекурсивная функция кодирования по дереву Хаффмана.
/// @param root указатель на полученный корень дерева
void makeCodes(symbol* root);

/// @brief Сортирует массив по убыванию значений `symbol.freq`
/// @param psym массив указателей на тип `symbol`
/// @param k длина массива
void descend_sort(symbol* psym[], int k);

#endif