#include <stdio.h>
#include <sys/stat.h>
#include <windows.h>
#include <locale.h>
#include "types.h"
#include "file_workers.h"

int main(int argc, char* argv[]) {

    enum mode mode = WRONG;
    enum info_display tinfo = TIME_ONLY;
    setlocale(LC_ALL, "Russian");
    if (argc < 4) {
        printf("Usage: arh [mode] [input path] [output path] [info option (default: time info)]\n");
        printf("Mode:\n");
        printf("   -c, --compress   Compress input file/dir to output file/dir\n");
        printf("   -e, --extract    Uncompress input file/dir compressed with \"arh\" to output file/dir\n");
        printf("Info options:\n");
        printf("   --full           Display all information and errors\n");
        printf("   --quiet          Do not display any information, except errors\n");
        return -1;
    } else if (argc == 5) {
        if (!strcmp(argv[4], "--quiet")) tinfo = NONE;
        else if (!strcmp(argv[4], "--full")) tinfo = FULL;
        else printf("\"%s\" is not an available option\n", argv[4]);
    }
    if (!strcmp(argv[1], "-c") || !strcmp(argv[1], "--compress")) mode = COMPRESS;
    else if (!strcmp(argv[1], "-e") || !strcmp(argv[1], "--extract")) mode = EXTRACT;

    int ret_code = 0;
    if (mode == COMPRESS) {
        struct stat s;
        if (!stat(argv[2], &s)) {           // получение свойств входного пути
            if (S_ISDIR(s.st_mode)) {       // входной путь - директория
                ret_code = directory_handler(mode, tinfo, argv);
            } else if (S_ISREG(s.st_mode)) {    // входной путь - файл
                FILE* f_in, * f_out;            //указатели на файлы
                f_in = fopen(argv[2], "rb");    //открываем входной файл
                if (f_in == NULL) {             //Обработка ошибок чтения файлов
                    perror("Err at input file");
                    return 1;
                }
                f_out = fopen(argv[3], "wb");   //открываем файл для записи сжатого файла
                if (f_out == NULL) {
                    perror("Err at output file");
                    return 2;
                }
                fputc(0, f_out);    // это сжатый файл
                ret_code = file_handler(f_in, f_out, mode, tinfo, argv);
                fclose(f_in);
                fclose(f_out);
            } else {
                printf("Input path is not a file or directory\n");
                return -3;
            }
        } else {
            perror("Err input path");
            return -2;
        }
    } else if (mode == EXTRACT) {
        ret_code = directory_handler(mode, tinfo, argv);
    } else {
        printf("\"%s\" is not an available mode\n", argv[1]);
        return -1;
    }
    printf("Worker return code: %3d\n", ret_code);
    return ret_code;
}