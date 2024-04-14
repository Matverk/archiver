#ifndef FILE_H5
#define FILE_H5

/// @brief Рeкурсивная функция создания дерева Хафмана.
/// @param psym по указателю значение нового пришедшего элемента. В данном случае, самого нижнего.
/// @param k общее количество символов
/// @return указатель на новую структуру типа symbol
symbol* makeTree(symbol* psym[], int k);

/// @brief Рекурсивная функция кодирования.
/// @param root указатель на полученный корень дерева
void makeCodes(symbol* root);

#endif
