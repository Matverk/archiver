#include <stdio.h>
#include <windows.h>
#include <sys/time.h>
#include "types.h"
#include "arch_logic.h"
#include "information.h"
#include "file_In_out.h"
#include "math_func.h"

double mtime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    double mt = t.tv_sec + t.tv_usec / 1.e6;
    return mt;
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Error with arguments\n");
        system("pause");
        return -1;
    }
    FILE* fp, * fp2, * fp3;     //указатели на файлы
    fp = fopen(argv[1], "rb");  //открываем конкретный файл

    //Обработка ошибок чтения файла
    if (fp == NULL) {
        perror("Err input file:");
        system("pause");
        return 1;
    }
    fp2 = fopen("temp", "wb");  //открываем файл для записи бинарного кода
    fp3 = fopen(argv[2], "wb"); //открываем файл для записи сжатого файла
    if (fp3 == NULL) {
        perror("Err output file:");
        system("pause");
        return 2;
    }

    unsigned int chh;   // в эту переменную читается информация из файла
    int k = 0;          //счётчик количества различных букв, уникальных символов
    int kk = 0;         // счётчик количества всех знаков в файле
    int kolvo[256] = { 0 };//инициализируем массив количества уникальных символов
    symbol simbols[256] = { 0 }; //инициализируем массив записей 
    symbol* psym[256];  //инициализируем массив указателей на записи
    int fsize2 = 0;     //счётчик количества (символов из) 0 и 1 в промежуточном файле temp

    double r0 = mtime();
    reading_from_file(fp, simbols, kolvo, &kk, &k);  //Эту функцию опишите самостоятельно
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
    makeCodes(root);  //вызов функции получения кода
    double mt2 = mtime();
    printf("tree: %f s\ncodes: %f s\n", mt1 - mt0, mt2 - mt1);

    rewind(fp); //возвращаем указатель в файле в начало файла
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
    double t0 = mtime();
    // writing_to_file(fp2, fp3, &fsize2);
    // printf("fsz2: %i\n", fsize2);
    double t1 = mtime();
    // printf("1 write: %f s\n", t1 - t0);

    rewind(fp);
    t0 = mtime();
    writing_to_file_simb(fp, fp3, simbols, &k, &fsize2);    // более быстрый способ без промежуточного файла
    t1 = mtime();
    printf("fsz2: %i\n", fsize2);
    show_information(simbols, &k, &kk, &fsize2);
    printf("2 write: %f s\n", t1 - t0);
    printf("all time: %f s", t1 - r0);

    fclose(fp);
    fclose(fp2);
    fclose(fp3);
    // system("pause");
    return 0;
}