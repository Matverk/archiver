#ifndef FILE_H1
#define FILE_H1

typedef struct sym {    // структура, в которой будет хрниться значение одного из пришедших символов. С помощью typedef вводим новый тип - symbol. Им мы обозначаем struct sym
    unsigned char ch;   //Непосредственно сам пришедший символ Его значение может быть от 0 до 255 (Коды ascii 2) Поэтому будем хранить его в 8 битном unsigned char. unsigned подчеркивает, что последний бит отведен под значение, а не по знак
    float freq;         //частота появления символа Имеет тип с плавающей запятой, т.к. может быть дробное значение
    char code[255];     //Новый код символа, полученный в результате построение дерева Хаффмана
    struct sym* left;   //Указатель на родительский элемент, расположенный слева. Необходим для построения дерева
    struct sym* right;  //Указатель на родительский элемент, расположенный справа.
} symbol;

union code {            //Объединение для записи кода символа в файл. Может принимать значение как unsigned char sym_to_write ИЛИ структуры byte. 
    unsigned char sym_to_write; //переменная содержащая код для записи в сжатый файл
    struct byte {               //представлена в виде битового поля - каждый unsigned символ занимает 1 бит
        unsigned b8 : 1;
        unsigned b7 : 1;
        unsigned b6 : 1;
        unsigned b5 : 1;
        unsigned b4 : 1;
        unsigned b3 : 1;
        unsigned b2 : 1;
        unsigned b1 : 1;
    } byte;
};

enum mode { WRONG, COMPRESS, EXTRACT };
enum info_display { NONE, FULL, TIME_ONLY };
#endif