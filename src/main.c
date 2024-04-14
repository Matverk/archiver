#include <stdio.h>
#include <math.h> 
#include "types.h"
#include "arch_logic.h"
#include "information.h"
#include "file_In_out.h"
#include "math_func.h"

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Error with arguments\n");
        return 1;
    }
    FILE* fp, * fp2, * fp3; //указатели на файлы
    fp = fopen(argv[1], "rb"); //открываем конкретный файл
    fp2 = fopen("temp.txt", "wb");//открываем файл для записи бинарного кода
    fp3 = fopen(argv[2], "wb");//открываем файл для записи сжатого файла

    unsigned int chh;  // в эту переменную читается информация из файла
    int k = 0; //счётчик количества различных букв, уникальных символов
    int kk = 0; // счётчтк количества всех знаков в файле
    int kolvo[256] = { 0 };//инициализируем массив количества уникальных символов
    symbol simbols[256] = { 0 }; //инициализируем массив записей 
    symbol* psym[256]; //инициализируем массив указателей на записи
    int fsize2 = 0;//счётчик количества символов из 0 и 1 в промежуточном файле temp

    //Обработка ошибок чтения файла
    if (fp == NULL) {
        puts("File not open");
        return 0;
    }

    reading_from_file(fp, simbols, kolvo, &kk, &k);  //Эту функцию опишите самостоятельно

    // Расчёт частоты встречаемости
    // Тут будет ваш код

    //В массив указателей psym заносим адреса записей
    // Тут будет ваш код (Пока не обязательно)

    //Сортировка по убыванию по частоте 
    // Тут будет ваш код


    //symbol* root = makeTree(psym, k);//вызов функции создания дерева Хафмана
    //makeCodes(root);//вызов функции получения кода

    rewind(fp);//возвращаем указатель в файле в начало файла
    //в цикле читаем исходный файл, и записываем полученные в функциях коды в промежуточный файл
    while ((chh = fgetc(fp)) != EOF) {
        for (int i = 0; i < k; i++)
            if (chh == simbols[i].ch) fputs(simbols[i].code, fp2);
    }
    fclose(fp2);

    //writing_to_file(fp2, fp3, simbols, kolvo, &kk, &k, &fsize2);  //Эту функцию опишите самостоятельно
    //set_information(simbols, &k, &kk, &fsize2); 

    fclose(fp);
    fclose(fp3);
    return 0;
}