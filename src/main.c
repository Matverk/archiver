#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "arch_logic.h"
#include "information.h"
#include "file_In_out.h"
#include "math_func.h"
#include "utils.h"

int main(int argc, char* argv[]) {

    if (argc != 4) {
        printf("Usage: arh [mode] [input file] [output file]\n");
        printf("Mode:\n");
        printf("   -c, --compress   Compress input file\n");
        printf("   -e, --extract    Uncompress input file, that has been compressed with \"arh\"\n");
        system("pause");
        return -1;
    }

    FILE* f_in, * f_out;     //указатели на файлы
    f_in = fopen(argv[2], "rb");  //открываем конкретный файл
    //Обработка ошибок чтения файла
    if (f_in == NULL) {
        perror("Err input file:");
        system("pause");
        return 1;
    }
    // FILE* fp2 = fopen("temp", "wb");  //открываем файл для записи бинарного кода
    f_out = fopen(argv[3], "wb"); //открываем файл для записи сжатого файла
    if (f_out == NULL) {
        perror("Err output file:");
        system("pause");
        return 2;
    }

    if (!strncmp(argv[1], "-c", 3) || !strncmp(argv[1], "--compress", 11)) {
        unsigned int chh;   // в эту переменную читается информация из файла
        int k = 0;          //счётчик количества различных букв, уникальных символов
        int kk = 0;         // счётчик количества всех знаков в файле
        symbol simbols[256] = { 0 }; //инициализируем массив записей 
        int kolvo[256] = { 0 };//инициализируем массив количества уникальных символов
        symbol* psym[256];  //инициализируем массив указателей на записи
        int fsize2 = 0;     //счётчик количества (символов из) 0 и 1 в промежуточном файле temp
        double r0 = mtime();
        reading_from_file(f_in, simbols, kolvo, &kk, &k);
        double r1 = mtime();
        printf("reading: %f s\n", r1 - r0);

        // Расчёт частоты встречаемости
        for (int i = 0; i < k; ++i) simbols[i].freq = (float)kolvo[i] / kk;

        // В массив указателей psym заносим адреса записей
        for (int i = 0; i < k; ++i) psym[i] = simbols + i;

        //Сортировка по убыванию по частоте
        descend_sort(psym, k);

        double mt0 = mtime();
        symbol* root = makeTree(psym, k);   //вызов функции создания дерева Хаффмана
        double mt1 = mtime();
        makeCodes(root);    //вызов функции получения кода
        double mt2 = mtime();
        printf("tree: %f s\ncodes: %f s\n", mt1 - mt0, mt2 - mt1);

        rewind(f_in); //возвращаем указатель в файле в начало файла
        //в цикле читаем исходный файл, и записываем полученные в функциях коды в промежуточный файл
        // double tm0 = mtime();
        // while ((chh = fgetc(fp)) != EOF) {   // можно перенести в writing_to_file()
        //     for (int i = 0; i < k; i++)
        //         if (chh == simbols[i].ch) fputs(simbols[i].code, fp2);
        // }
        // double tm1 = mtime();
        // fclose(fp2);
        // printf("fp2: %f s\n", tm1 - tm0);

        // fp2 = fopen("temp", "rb");
        // double t0 = mtime();
        // writing_to_file(fp2, f_out, &fsize2);
        // printf("fsz2: %i\n", fsize2);
        // double t1 = mtime();
        // printf("1 write: %f s\n", t1 - t0);
        rewind(f_in);
        double t0 = mtime();
        writing_to_file_simb(f_in, f_out, simbols, &k, &fsize2);    // более быстрый способ без промежуточного файла
        double t1 = mtime();
        show_information(simbols, &k, &kk, &fsize2);
        printf("2 write: %f s\n", t1 - t0);
        printf("all time: %f s", t1 - r0);
    }
    else if (!strncmp(argv[1], "-e", 3) || !strncmp(argv[1], "--extract", 10)) {

    }
    fclose(f_in);
    // fclose(fp2);
    fclose(f_out);
    // system("pause");
    return 0;
}