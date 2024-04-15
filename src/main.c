#include <stdio.h>
#include <windows.h>
#include "types.h"
#include "arch_logic.h"
#include "information.h"
#include "file_In_out.h"
#include "math_func.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Error with arguments\n");
        system("pause");
        return 1;
    }
    FILE* fp, * fp2, * fp3;     //указатели на файлы
    fp = fopen(argv[1], "rb");  //открываем конкретный файл

    //Обработка ошибок чтения файла
    if (fp == NULL) {
        perror("Err");
        system("pause");
        return 2;
    }
    fp2 = fopen("temp", "wb");  //открываем файл для записи бинарного кода
    fp3 = fopen(argv[2], "wb"); //открываем файл для записи сжатого файла

    unsigned int chh;   // в эту переменную читается информация из файла
    int k = 0;          //счётчик количества различных букв, уникальных символов
    int kk = 0;         // счётчик количества всех знаков в файле
    int kolvo[256] = { 0 };//инициализируем массив количества уникальных символов
    symbol simbols[256] = { 0 }; //инициализируем массив записей 
    symbol* psym[256];  //инициализируем массив указателей на записи
    int fsize2 = 0;     //счётчик количества (символов из) 0 и 1 в промежуточном файле temp

    reading_from_file(fp, simbols, kolvo, &kk, &k);  //Эту функцию опишите самостоятельно

    // Расчёт частоты встречаемости
    for (int i = 0; i < k; ++i) simbols[i].freq = (float)kolvo[i] / kk;

    // В массив указателей psym заносим адреса записей
    for (int i = 0; i < k; ++i) psym[i] = simbols + i;

    //Сортировка по убыванию по частоте
    descend_sort(psym, k);

    symbol* root = makeTree(psym, k);   //вызов функции создания дерева Хаффмана
    makeCodes(root);  //вызов функции получения кода

    rewind(fp); //возвращаем указатель в файле в начало файла
    //в цикле читаем исходный файл, и записываем полученные в функциях коды в промежуточный файл
    while ((chh = fgetc(fp)) != EOF) {
        for (int i = 0; i < k; i++)
            if (chh == simbols[i].ch) fputs(simbols[i].code, fp2);
    }
    fclose(fp2);

    fp2 = fopen("temp", "rb");
    writing_to_file(fp2, fp3, &fsize2);  //Эту функцию опишите самостоятельно
    show_information(simbols, &k, &kk, &fsize2);

    fclose(fp);
    fclose(fp2);
    fclose(fp3);
    // system("pause");
    return 0;
}