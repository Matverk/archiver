#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "arch_logic.h"
#include "information.h"
#include "compress_file_in_out.h"
#include "extract_file_in_out.h"
#include "math_func.h"
#include "utils.h"

int main(int argc, char* argv[]) {

    enum info_display tinfo = TIME_ONLY;
    if (argc < 4) {
        printf("Usage: arh [mode] [input file] [output file] [info option]\n");
        printf("Mode:\n");
        printf("   -c, --compress   Compress input file to output file\n");
        printf("   -e, --extract    Uncompress input file compressed with \"arh\" to output file\n");
        printf("Info options:\n");
        printf("   --full           Display all information and errors\n");
        printf("   --quiet          Do not display any information, except errors\n");
        // system("pause");
        return -1;
    }
    else if (argc == 5) {
        if (!strcmp(argv[4], "--quiet")) tinfo = NONE;
        else if (!strcmp(argv[4], "--full")) tinfo = FULL;
        else printf("\"%s\" is not an available option\n", argv[4]);
    }

    FILE* f_in, * f_out;            //указатели на файлы
    f_in = fopen(argv[2], "rb");    //открываем входной файл
    //Обработка ошибок чтения файлов
    if (f_in == NULL) {
        perror("Err input file:");
        // system("pause");
        return 1;
    }
    f_out = fopen(argv[3], "wb");   //открываем файл для записи сжатого файла
    if (f_out == NULL) {
        perror("Err output file:");
        // system("pause");
        return 2;
    }

    int uniqk = 0;      // счётчик количества различных букв, уникальных символов
    symbol simbols[256] = { 0 };    // массив записей
    if (!strcmp(argv[1], "-c") || !strcmp(argv[1], "--compress")) {
        int kk = 0;         // счётчик количества всех знаков в файле
        int kolvo[256] = { 0 };     // массив количеств уникальных символов
        symbol* psym[256];  // инициализируем массив указателей на записи
        int fsize2 = 0;     // счётчик количества бит в сжатом файле
        double r0 = mtime();
        reading_from_file(f_in, simbols, kolvo, &kk, &uniqk);
        double r1 = mtime();

        // Расчёт частоты встречаемости
        for (int i = 0; i < uniqk; ++i) simbols[i].freq = (float)kolvo[i] / kk;

        // В массив указателей psym заносим адреса записей
        for (int i = 0; i < uniqk; ++i) psym[i] = simbols + i;

        //Сортировка по убыванию по частоте
        descend_sort(psym, uniqk);

        double mt0 = mtime();
        symbol* root = makeTree(psym, uniqk);   //создание дерева Хаффмана
        if (root == NULL) return -10;
        double mt1 = mtime();
        makeCodes(root);    //получение кодов
        double mc2 = mtime();
        rewind(f_in);   //возвращаем указатель в файле в начало файла
        // double ts0 = mtime();
        // compress_to_file(f_in, f_out, simbols, uniqk, &fsize2);    // медленно
        // double ts1 = mtime();
        // rewind(f_in);
        double t0 = mtime();
        write_code_table(f_out, simbols, uniqk);
        compress_to_file_simb(f_in, f_out, simbols, uniqk, &fsize2);    // более быстрый способ без промежуточного файла
        double t1 = mtime();
        if (tinfo != NONE) {
            if (tinfo == FULL) show_information(simbols, uniqk, kk, fsize2);
            printf("Time info:\nreading: %f s\n", r1 - r0);
            printf("make tree: %f s\nmake codes: %f s\n", mt1 - mt0, mc2 - mt1);
            // printf("1 write: %f s\n", ts1 - ts0);
            printf("fast write: %f s\n", t1 - t0);
            printf("all time: %f s\n", t1 - r0);
        }
    }
    else if (!strcmp(argv[1], "-e") || !strcmp(argv[1], "--extract")) {
        double te0 = mtime();
        uniqk = read_code_table(f_in, simbols);
        if (extract_from_file(f_in, f_out, simbols, uniqk)) return -11;
        double te1 = mtime();
        if (tinfo != NONE) printf("Extracted to \"%s\" in %f s\n", argv[3], te1 - te0);
    }
    fclose(f_in);
    fclose(f_out);
    // system("pause");
    return 0;
}