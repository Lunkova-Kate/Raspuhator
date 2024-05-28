#include "haff.h"

//����������: ������� ����
// ������� ��� ������ ����� � �������� ������
unsigned char* read_file_binary(const char* filename, long* file_size) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    rewind(file);
    // �� ������
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

//���������� : ��������� ������� ������:
    // ������� ��� ��������� ������ ������ ��� ����������
int compare_byte_frequencies(const void* a, const void* b) {
    long freq_a = ((ByteFrequency*)a)->frequency;
    long freq_b = ((ByteFrequency*)b)->frequency;
    return (freq_b > freq_a) - (freq_b < freq_a);
}

// ������� ��� �������� � ���������� ������ ������
void count_and_sort_byte_frequencies(const unsigned char* data, long file_size, ByteFrequency* frequencies) {
    memset(frequencies, 0, BYTE_RANGE * sizeof(ByteFrequency));

    // ������� ������
    for (long i = 0; i < file_size; ++i) {
        //�������� ������ � ����������� �������
        unsigned char currentByte = data[i];
        frequencies[currentByte].byte = currentByte;
        frequencies[currentByte].frequency++;

    }
    qsort(frequencies, BYTE_RANGE, sizeof(ByteFrequency), compare_byte_frequencies);
}
//���������� : ��������� ������



/*
   // ������� ��� �������� ���������� ����� � ������ ��������
int count_nodes_in_huffman_tree(const ByteFrequency* frequencies) {
    int nodes_count = 0;
    for (int i = 0; i < BYTE_RANGE; ++i) {
        if (frequencies[i].frequency > 0) {
            nodes_count++;
        }
    }
    return nodes_count;
}*/
// ������� ��� �������� ������ ���� ������ ��������
Node* createNode(unsigned char byte, long frequency) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->byte = byte;
    node->frequency = frequency;
    node->left = node->right = NULL;
    return node;
}
// ������� ��� ��������� ����� �� �������
int compareNodes(const void* a, const void* b) {
    Node* nodeA = *(Node**)a;
    Node* nodeB = *(Node**)b;
    return (nodeA->frequency > nodeB->frequency) - (nodeA->frequency < nodeB->frequency);
}
// ������� ��� ���������� ������ ��������
Node* buildHuffmanTree(ByteFrequency* frequencies, int size) {
    Node** nodes = (Node**)malloc(size * sizeof(Node*));
    int count = 0;

    for (int i = 0; i < size; ++i) {
        if (frequencies[i].frequency > 0) {
            nodes[count++] = createNode(frequencies[i].byte, frequencies[i].frequency);
        }
    }

    if (count == 1) {
        // ���� ������ ���� ����, ������� ���
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

        // ���������� ���������� ������  �� ���� ������� �����
        for (int i = 2; i < count; ++i) {
            nodes[i - 2] = nodes[i];
        }
        nodes[count - 2] = new_node;
        nodes[count - 1] = NULL; // ������ ���������

        count--;
    }

    Node* root = nodes[0];
    free(nodes);

    return root;
}
// ������� ��� ����������� ������ ��������

// ������� ��� ������� ������ ��������
void freeHuffmanTree(Node* node) {
    if (node == NULL) return;
    freeHuffmanTree(node->left);
    freeHuffmanTree(node->right);
    free(node);
}
// ������� ��� ������ ������ �������� (��� �������)
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
//���������� : ��������� ���� ��������
// ������� ��� ����������� ������ ��������
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
    haffmancode[depth] = '\0'; // ���������� haffmancode � ����������� ���������
}
// ������� ��� ������ ������� ����� � ������ � ����
void writeFileSizeToFile(FILE* file, uint32_t fileSize) {
    // ���������� ������ ����� � ������ � ����
    fwrite(&fileSize, sizeof(uint32_t), 1, file);
}



//���������� : ������������ ����(������� + ������ ��� �������� �����)

// ������� ��� ����������� �����
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

    // �������� ������� ����� ��������
    char haffmancode[BYTE_RANGE] = { 0 };
    encodeHuffmanTree(huffmanTree, haffmancode, codes, 0);

    // ����������� �����
    for (long i = 0; i < fileSize; ++i) {
        const char* code = codes[data[i]];
        fwrite(code, strlen(code), 1, outputFile);
    }

    fclose(outputFile);
    free(data);
}

//������������ : ������� ������� ������
// 
// ������� ��� ������ ������� ����� �� ������ �����
uint32_t readFileSizeFromFile(FILE* file) {
    uint32_t fileSize;
    fread(&fileSize, sizeof(uint32_t), 1, file);
    return fileSize;
}
// ������� ��� ������������� �����
// ������� ��� ������������� �����
void decodeFile(const char* inputFilename, const char* outputFilename, Node* huffmanTree) {
    FILE* inputFile = fopen(inputFilename, "rb");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return;
    }

    // ������ ������� ����� �� ������ �����
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
        // ����������� �� ������ �������� � ����������� �� ����
        currentNode = (byte == '0') ? currentNode->left : currentNode->right;

        // ���� ��������� ���� ������, ���������� ���� � ������������ � �����
        if (currentNode->left == NULL && currentNode->right == NULL) {
            fwrite(&currentNode->byte, sizeof(unsigned char), 1, outputFile);
            currentNode = huffmanTree;
            decodedSize++;
        }
    }

    fclose(inputFile);
    fclose(outputFile);
}