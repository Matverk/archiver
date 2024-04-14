#ifndef FILE_H3
#define FILE_H3

/// @brief Чтение и группировка символов
/// @param fp входной бинарный файл
/// @param simbols выходной массив уникальных символов
/// @param kolvo выходной массив частот символов
/// @param allk количество прочитанных символов
/// @param uniqk количество уникальных символов
void reading_from_file(FILE* fp, symbol* simbols, int* kolvo, int* allk, int* uniqk);


void writing_to_file(FILE*, FILE*, symbol*, int*, int*, int*, int*);
#endif
