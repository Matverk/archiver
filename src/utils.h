#pragma once
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <windows.h>
#include "types.h"

/// @brief Получение текущего времени системы
/// @return Текущее время в секундах
double mtime();

/// @brief Создаёт все вложенные директории по пути, если не существуют
/// @param path путь создания
void recurse_mkdir(char* path);

/// @brief Создаёт файл по пути и открывает его
/// @param path путь
/// @param mode режим (аналогичен `fopen()`)
/// @return указатель на поток
FILE* fopen_mkdir(char* path, char* mode);

/// @brief Определяет путь до временной директории и сохраняет на кучу
/// @return указатель на строку в памяти
char* tmppath();