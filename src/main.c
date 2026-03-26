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

    if (mode == COMPRESS) {
        struct stat s;
        if (!stat(argv[2], &s)) {           // получение свойств входного пути
            if (S_ISDIR(s.st_mode)) {       // входной путь - директория
                directory_handler(mode, tinfo, argv);
            } else if (S_ISREG(s.st_mode)) {    // входной путь - файл
                FILE* f_in, * f_out;            //указатели на файлы
                f_in = fopen(argv[2], "rb");    //открываем входной файл
                check_log_err_exit(f_in != NULL, "Input file opening", 1, 31);

                f_out = fopen(argv[3], "wb");   //открываем файл для записи сжатого файла
                check_log_err_exit(f_out != NULL, "Output file opening", 1, 32);
                fputc(ARCHIVE_IS_FILE, f_out);    // это сжатый файл
                file_handler(f_in, f_out, mode, tinfo, argv);
                fclose(f_in);
                fclose(f_out);
            } else check_log_err_exit(0, "Input path is not a file or directory", 1, 33);
        } else check_log_err_exit(0, "Get input path stats", 1, 34);
    } else if (mode == EXTRACT) directory_handler(mode, tinfo, argv);
    else {
        printf("\"%s\" is not an available mode\n", argv[1]);
        return -2;
    }
    return 0;
}