#ifndef FILE_H2
#define FILE_H2
#include <math.h>
#include "types.h"

/// @brief 
/// @param simbols массив символов - сообщение
/// @param k количество символов в сообщении
/// @return 
float entropy_count(symbol* simbols, int* k);

/// @brief 
/// @param fsize_2 количество 0-й и 1-ц в сжатом файле
/// @param allk количество символов в сообщении
/// @return 
double bit_costs(int* fsize_2, int allk);

#endif
