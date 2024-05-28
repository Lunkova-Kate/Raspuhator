#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//����������� ��������(����������� � ������ ������)

#define BYTE_RANGE 256 // ���������� ��������� ������ (�� 0 �� 255)

// ��������� ��� �������� ������� ������
typedef struct {
    unsigned char byte;
    long frequency;
} ByteFrequency;
// ��������� ��� �������� ���� ������ ��������
typedef struct Node {
    unsigned char byte;
    long frequency;
    struct Node* left, * right;
} Node;

//����������: ������� ����
   /// ������� ��� ������ ����� � �������� ������
unsigned char* read_file_binary(const char* filename, long* file_size);

//���������� : ��������� ������� ������
int compare_byte_frequencies(const void* a, const void* b);
/// ������� ��� �������� � ���������� ������ ������
void count_and_sort_byte_frequencies(const unsigned char* data, long file_size, ByteFrequency* frequencies);
//���������� : ��������� ������
// 

///������������� ������
Node* createNode(unsigned char byte, long frequency);
/// ������� ��� ��������� ����� �� �������
int compareNodes(const void* a, const void* b);
/// ������� ��� ���������� ������ ��������
Node* buildHuffmanTree(Node** nodes, int size);
/// ������� ��� ������� ������ ��������
void freeHuffmanTree(Node* node);
/// ������� ��� ������ ������ �������� (��� �������)
void printHuffmanTree(Node* node, int depth);
/// ������� ��� ����������� ������ ��������
void encodeHuffmanTree(Node* node, char* code, char** codes, int depth);
// ������� ��� ������ ������� ����� � ������ � ����
void writeFileSizeToFile(FILE* file, uint32_t fileSize);
//���������� : ������������ ����(������� + ������ ��� �������� �����)
/// ������� ��� ����������� ����� � �������������� ����� ��������
void encodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree, char** codes);

//������������ : 
// ������� ��� ������ ������� ����� �� ������ �����
uint32_t readFileSizeFromFile(FILE* file);

void decodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree);