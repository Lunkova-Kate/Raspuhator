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
// Функция для чтения файла в бинарном режиме
unsigned char* read_file_binary(const char* filename, long* file_size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);
    unsigned char* buffer = (unsigned char*)malloc(sizeof(unsigned char) * (*file_size));
    if (buffer == NULL) {
        fprintf(stderr, "Memory error");
        fclose(file);
        return NULL;
    }
    fread(buffer, 1, *file_size, file);
    fclose(file);
    return buffer;
}

//Кодировщик : построить таблицу частот:
// Функция для сравнения частот байтов для сортировки
int compare_byte_frequencies(const void* a, const void* b) {
    long freq_a = ((ByteFrequency*)a)->frequency;
    long freq_b = ((ByteFrequency*)b)->frequency;
    return (freq_b > freq_a) - (freq_b < freq_a);
}

// Функция для подсчета и сортировки частот байтов
void count_and_sort_byte_frequencies(const unsigned char* data, long file_size, ByteFrequency* frequencies) {
    memset(frequencies, 0, BYTE_RANGE * sizeof(ByteFrequency));

    // Подсчет частот
    for (long i = 0; i < file_size; ++i) {
        //получили байтик и увеличиваем частоту
        unsigned char currentByte = data[i];
        frequencies[currentByte].byte = currentByte;
        frequencies[currentByte].frequency++;

    }
    qsort(frequencies, BYTE_RANGE, sizeof(ByteFrequency), compare_byte_frequencies);
}
//Кодировщик : построить дерево
// Функция для создания нового узла дерева Хаффмана
Node* createNode(unsigned char byte, long frequency) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->byte = byte;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}
// Функция для сравнения узлов по частоте
int compareNodes(const void* a, const void* b) {
    Node* nodeA = *(Node**)a;
    Node* nodeB = *(Node**)b;
    return (nodeA->frequency > nodeB->frequency) - (nodeA->frequency < nodeB->frequency);
}
// Функция для построения дерева Хаффмана
Node* buildHuffmanTree(ByteFrequency* frequencies, int size) {
    Node** nodes = (Node**)malloc(size * sizeof(Node*));
    int count = 0;

    for (int i = 0; i < size; ++i) {
        if (frequencies[i].frequency > 0) {
            nodes[count++] = createNode(frequencies[i].byte, frequencies[i].frequency);
        }
    }

    if (count == 1) {
        // Если только один узел, вернуть его
        Node* root = nodes[0];
        free(nodes);
        return root;
    }

    while (count > 1) {
        qsort(nodes, count, sizeof(Node*), compareNodes);

        Node* left = nodes[0];
        Node* right = nodes[1];
        Node* new_node = createNode(0, left->frequency + right->frequency);
        new_node->left = left;
        new_node->right = right;

        // Перемещаем оставшиеся узелки  на одну позицию влево
        for (int i = 2; i < count; ++i) {
            nodes[i - 2] = nodes[i];
        }
        nodes[count - 2] = new_node;
        nodes[count - 1] = NULL; // Чистим последнее

        count--;
    }

    Node* root = nodes[0];
    free(nodes);

    return root;
}
// Функция для кодирования дерева Хаффмана

// Функция для очистки дерева Хаффмана
void freeHuffmanTree(Node* node) {
    if (node == NULL) return;
    freeHuffmanTree(node->left);
    freeHuffmanTree(node->right);
    free(node);
}
// Функция для вывода дерева Хаффмана (для отладки)
void printHuffmanTree(Node* node, int depth) {
    if (node == NULL) return;
    for (int i = 0; i < depth; ++i) printf("  ");
    if (node->byte != 0) {
        printf("'%c': %ld\n", node->byte, node->frequency);
    }
    else {
        printf("%ld\n", node->frequency);
    }
    printHuffmanTree(node->left, depth + 1);
    printHuffmanTree(node->right, depth + 1);
}
//Кодировщик : построить коды Хаффмана
// Функция для кодирования дерева Хаффмана
void encodeHuffmanTree(Node* node, char* haffmancode, char** codes, int depth) {
    if (node == NULL) return;
    if (node->left == NULL && node->right == NULL) {
        codes[node->byte] = (char*)malloc(strlen(haffmancode) + 1);
        strcpy(codes[node->byte], haffmancode);
    }
    else {
        haffmancode[depth] = '0';
        encodeHuffmanTree(node->left, haffmancode, codes, depth + 1);
        haffmancode[depth] = '1';
        encodeHuffmanTree(node->right, haffmancode, codes, depth + 1);
    }
    haffmancode[depth] = '\0'; // Возвращаем haffmancode к предыдущему состоянию
}
// Функция для записи размера файла в байтах в файл
void writeFileSizeToFile(FILE* file, uint32_t fileSize) {
    // Записываем размер файла в байтах в файл
    fwrite(&fileSize, sizeof(uint32_t), 1, file);
}
//Кодировщик : закодировать файл(таблица + данные без упаковки битов)
// Функция для кодирования файла
void encodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree, char** codes) {
    long fileSize;
    unsigned char* data = read_file_binary(inputFilename, &fileSize);
    if (data == NULL) {
        return;
    }

    FILE* outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        free(data);
        return;
    }

    writeFileSizeToFile(outputFile, fileSize);

    // Создание таблицы кодов Хаффмана
    char haffmancode[BYTE_RANGE] = { 0 };
    encodeHuffmanTree(huffmanTree, haffmancode, codes, 0);

    // Кодирование файла
    for (long i = 0; i < fileSize; ++i) {
        const char* code = codes[data[i]];
        if (code != NULL) {
            fwrite(code, strlen(code), 1, outputFile);
        }
    }

    fclose(outputFile);
    free(data);
}

//Декодировщик : считать таблицу частот
//
// Функция для чтения размера файла из начала файла
uint32_t readFileSizeFromFile(FILE* file) {
    uint32_t fileSize;
    fread(&fileSize, sizeof(uint32_t), 1, file);
    return fileSize;
}
// Функция для декодирования файла
// Функция для декодирования файла
void decodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree) {
    if (huffmanTree == NULL) {
        fprintf(stderr, "Huffman tree is NULL");
        return;
    }

    FILE* inputFile = fopen(inputFilename, "rb");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return;
    }

    // Чтение размера файла из начала файла
    uint32_t fileSize = readFileSizeFromFile(inputFile);

    FILE* outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    Node* currentNode = huffmanTree;
    unsigned char byte;
    uint32_t decodedSize = 0;
    while (decodedSize < fileSize && fread(&byte, sizeof(unsigned char), 1, inputFile) == 1) {
        // Перемещение по дереву Хаффмана в зависимости от бита
        currentNode = (byte == '0') ? currentNode->left : currentNode->right;

        // Если достигнут лист дерева, записываем байт и возвращаемся к корню
        if (currentNode->left == NULL && currentNode->right == NULL) {
            fwrite(&currentNode->byte, sizeof(unsigned char), 1, outputFile);
            currentNode = huffmanTree;
            decodedSize++;
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}


// Функция для упаковки битов в байты и записи в файл
void packBitsAndWriteToFile(const char* outputFilename, const char** codes, const unsigned char* data, long fileSize) {
    FILE* outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return;
    }

    // Записываем размер файла в байтах в файл
    writeFileSizeToFile(outputFile, fileSize);

    // Упаковка битов в байты и запись в файл
    unsigned char packedByte = 0;
    int bitCount = 0;
    for (long i = 0; i < fileSize; ++i) {
        const char* code = codes[data[i]];
        for (size_t j = 0; j < strlen(code); ++j) {
            packedByte <<= 1; // Сдвиг на один бит влево
            if (code[j] == '1') {
                packedByte |= 1; // Установка младшего бита в 1
            }
            bitCount++;
            if (bitCount == 8) {
                // Записываем полный байт в файл
                fwrite(&packedByte, sizeof(unsigned char), 1, outputFile);
                packedByte = 0;
                bitCount = 0;
            }
        }
    }

    // Если остались незаписанные биты, дополним их нулями и запишем
    if (bitCount > 0) {
        packedByte <<= (8 - bitCount); // Дополнить нулями до полного байта
        fwrite(&packedByte, sizeof(unsigned char), 1, outputFile);
    }

    fclose(outputFile);
}

// Функция для чтения упакованных байт из файла и распаковки битов
void readPackedBytesAndUnpackBits(const char* inputFilename, const char* outputFilename, Node* huffmanTree) {
    if (huffmanTree == NULL) {
        fprintf(stderr, "Huffman tree is NULL");
        return;
    }

    FILE* inputFile = fopen(inputFilename, "rb");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return;
    }

    // Чтение размера файла из начала файла
    uint32_t fileSize = readFileSizeFromFile(inputFile);

    FILE* outputFile = fopen(outputFilename, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    Node* currentNode = huffmanTree;
    unsigned char byte;
    uint32_t decodedSize = 0;
    while (decodedSize < fileSize && fread(&byte, sizeof(unsigned char), 1, inputFile) == 1) {
        for (int i = 7; i >= 0; --i) {
            // Проверка каждого бита в байте
            currentNode = (byte & (1 << i)) ? currentNode->right : currentNode->left;

            // Если достигнут лист дерева, записываем байт и возвращаемся к корню
            if (currentNode->left == NULL && currentNode->right == NULL) {
                fwrite(&currentNode->byte, sizeof(unsigned char), 1, outputFile);
                currentNode = huffmanTree;
                decodedSize++;
            }

            if (decodedSize >= fileSize) {
                break;
            }
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}
int main() {
    const char* inputFilename = "in.txt";
    const char* encodedFilename = "out.bin";
    const char* decodedFilename = "decoded.txt";
    const char* packedFilename = "packed.bin";
    long fileSize;
    unsigned char* data = read_file_binary(inputFilename, &fileSize);
    if (data == NULL) {
        return 1;
    }
    ByteFrequency frequencies[BYTE_RANGE];
    count_and_sort_byte_frequencies(data, fileSize, frequencies);
    Node* huffmanTree = buildHuffmanTree(frequencies, BYTE_RANGE);

    char* codes[BYTE_RANGE] = { 0 };
    char haffmancode[BYTE_RANGE] = { 0 };
    encodeHuffmanTree(huffmanTree, haffmancode, codes, 0);
    encodeFile(inputFilename, encodedFilename, huffmanTree, codes);
    decodeFile(encodedFilename, decodedFilename, huffmanTree);

    packBitsAndWriteToFile(packedFilename, (const char**)codes, data, fileSize);
    readPackedBytesAndUnpackBits(packedFilename, "unpacked.txt", huffmanTree);
    freeHuffmanTree(huffmanTree);
    for (int i = 0; i < BYTE_RANGE; ++i) {
        free(codes[i]);
    }
    free(data);
    return 0;
}
