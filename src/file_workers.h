#pragma once
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "types.h"
#include "utils.h"
#include "arch_logic.h"
#include "compress_file_in_out.h"
#include "extract_file_in_out.h"
#include "information.h"

#define TMP_ARCHDIR_FNAME ".arch.dir"   // имя временного архивного несжатого файла директории
#define TOC_BUFSIZE_STEP 100            // шаг длины оглавления в дин.памяти (число записей)
#define TRANSFER_BUF_SIZE 1024*50       // шаг буфера для копирования файлов, байт
#define ARCHIVE_IS_FILE 0           // такой первый байт сжатого архива означает, что это был 1 файл
#define ARCHIVE_IS_DIR 1            // означает, что это была директория

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
/// @param tmp_path путь до директории для временного файла
/// @param tinfo режим вывода информации
/// @return указатель на созданный архивный файл
FILE* create_directory_archfile(char* path, char* tmp_path, enum info_display tinfo);

/// @brief Дозаписывает оглавление для директории
/// @param path путь до директории относительно вызова программы
/// @param toc указатель на массив оглавления
/// @param toc_rootpath укороченный путь до директории
/// @param toc_len указатель на число записей в оглавлении
/// @param toc_alloc_len указатель на выделенный размер оглавления в куче (кол-во записей)
void archive_directory(char* path, char* toc_rootpath, toc_entry** toc[], unsigned* toc_len, unsigned* toc_alloc_len);

/// @brief Разархивирует директорию, описанную в несжатом файле
/// @param fname путь до архивного несжатого файла
/// @param tinfo режим вывода информации
/// @param argv аргументы программы
/// @return `0`, если всё ОК
int extract_directory_archfile(char* fname, enum info_display tinfo, char* argv[]);

/// @brief Сжимает или расжимает директорию
/// @param mode режим: сжать/разжать
/// @param tinfo режим вывода информации
/// @param argv аргументы программы
/// @return `0`, если всё ОК
int directory_handler(enum mode mode, enum info_display tinfo, char* argv[]);