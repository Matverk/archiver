#pragma once
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <windows.h>
#include "types.h"

#define TMP_UNIX_PATH "/tmp"          // путь временной директории для не Windows

/* Проверяет условие. Выводит сообщение при ошибке (если условие неверно), пользовательский текст,
её описание, место выхода и завершает программу с кодом `_code` */
#define check_log_err_exit(_condition, _op, _errno_presence, _code) \
    if (!(_condition)) {\
        fprintf(stderr, "Error at '%s' %s", __func__, _op);\
        if (_errno_presence) fprintf(stderr, ": %s", strerror(errno));\
        fprintf(stderr, "\nExit code: %3d\n", _code);\
        exit(_code); }

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