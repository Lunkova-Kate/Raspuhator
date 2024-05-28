#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Распухатель Хаффмана(проверяется в ручном режиме)

#define BYTE_RANGE 256 // Количество возможных байтов (от 0 до 255)

// Структура для хранения частоты байтов
typedef struct {
    unsigned char byte;
    long frequency;
} ByteFrequency;
// Структура для хранения узла дерева Хаффмана
typedef struct Node {
    unsigned char byte;
    long frequency;
    struct Node* left, * right;
} Node;

//Кодировщик: считать файл
   /// Функция для чтения файла в бинарном режиме
unsigned char* read_file_binary(const char* filename, long* file_size);

//Кодировщик : построить таблицу частот
int compare_byte_frequencies(const void* a, const void* b);
/// Функция для подсчета и сортировки частот байтов
void count_and_sort_byte_frequencies(const unsigned char* data, long file_size, ByteFrequency* frequencies);
//Кодировщик : построить дерево
// 

///инициализация дерева
Node* createNode(unsigned char byte, long frequency);
/// Функция для сравнения узлов по частоте
int compareNodes(const void* a, const void* b);
/// Функция для построения дерева Хаффмана
Node* buildHuffmanTree(Node** nodes, int size);
/// Функция для очистки дерева Хаффмана
void freeHuffmanTree(Node* node);
/// Функция для вывода дерева Хаффмана (для отладки)
void printHuffmanTree(Node* node, int depth);
/// Функция для кодирования дерева Хаффмана
void encodeHuffmanTree(Node* node, char* code, char** codes, int depth);
// Функция для записи размера файла в байтах в файл
void writeFileSizeToFile(FILE* file, uint32_t fileSize);
//Кодировщик : закодировать файл(таблица + данные без упаковки битов)
/// Функция для кодирования файла с использованием кодов Хаффмана
void encodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree, char** codes);

//Декодировщик : 
// Функция для чтения размера файла из начала файла
uint32_t readFileSizeFromFile(FILE* file);

void decodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree);