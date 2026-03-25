#pragma once
#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <dirent.h>
#include "types.h"
#include "arch_logic.h"
#include "compress_file_in_out.h"
#include "extract_file_in_out.h"
#include "information.h"

#define TMP_ARCHDIR_FNAME ".arch.dir"
#define TOC_BUFSIZE_STEP 100
#define TRANSFER_BUF_SIZE 1024*3

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

/// @brief Сжимает или разжимает один файл
/// @param f_in входной файл
/// @param f_out выходной файл
/// @param mode режим: сжать/разжать
/// @param tinfo режим вывода информации
/// @param argv аргументы программы
/// @return `0`, если всё ОК
int file_handler(FILE* f_in, FILE* f_out, enum mode mode, enum info_display tinfo, char* argv[]);

/// @brief Описывает всё содержимое (непустые папки) директории в одном файле
/// @param path путь до директории
/// @return указатель на созданный архивный файл
FILE* create_directory_archfile(char* path);

/// @brief Дозаписывает оглавление для директории
/// @param path путь до директории
/// @param toc указатель на массив оглавления
/// @param toc_len указатель на число записей в оглавлении
/// @param toc_alloc_len указатель на выделенный размер оглавления в куче (кол-во записей)
void archive_directory(char* path, toc_entry** toc[], unsigned* toc_len, unsigned* toc_alloc_len);

/// @brief Сжимает или расжимает директорию
/// @param argv аргументы программы
/// @return `0`, если всё ОК
int directory_handler(enum mode mode, enum info_display tinfo, char* argv[]);