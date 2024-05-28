#include "haff.h"


/*int main() {
    FILE* file = fopen("input.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char command;
    fscanf(file, "%c", &command);

    if (command == 'c') {
        // Кодировщик
        char inputFilename[1000];
        char outputFilename[1000];
        fscanf(file, "%s", inputFilename);
        fscanf(file, "%s", outputFilename);

        long fileSize;
        unsigned char* data = read_file_binary(inputFilename, &fileSize);
        if (data == NULL) {
            return 1;
        }

        ByteFrequency frequencies[BYTE_RANGE];
        count_and_sort_byte_frequencies(data, fileSize, frequencies);

        Node* huffmanTree = buildHuffmanTree(frequencies, BYTE_RANGE);

        char haffmancode[BYTE_RANGE] = { 0 };
        char* codes[BYTE_RANGE] = { 0 };
        encodeHuffmanTree(huffmanTree, haffmancode, codes, 0);

        encodeFile(inputFilename, outputFilename, huffmanTree, codes);

        free(data);
        for (int i = 0; i < BYTE_RANGE; ++i) {
            free(codes[i]);
        }
        freeHuffmanTree(huffmanTree);
    }
    else if (command == 'd') {
        // Декодировщик
        char inputFilename[256];
        char outputFilename[256];
        fscanf(file, "%s", inputFilename);
        fscanf(file, "%s", outputFilename);

        long fileSize;
        unsigned char* data = read_file_binary(inputFilename, &fileSize);
        if (data == NULL) {
            return 1;
        }

        ByteFrequency frequencies[BYTE_RANGE];
        count_and_sort_byte_frequencies(data, fileSize, frequencies);

        Node* huffmanTree = buildHuffmanTree(frequencies, BYTE_RANGE);

        decodeFile(inputFilename, outputFilename, huffmanTree);

        free(data);
        freeHuffmanTree(huffmanTree);
    }
    else {
        /*printf("Unknown command\n");
    }

    fclose(file);
    return 0;
}*/
    int main() {


        uint32_t fileSize;
        unsigned char* data = read_file_binary("in.txt", &fileSize);
        if (data == NULL) {
            return 1;
        }

        ByteFrequency frequencies[BYTE_RANGE];
        count_and_sort_byte_frequencies(data, fileSize, frequencies);

        printf("1) :\n");
        for (long i = 0; i < fileSize; ++i) {
            printf("%02X ", data[i]);
        }
        printf("\n");

        printf("2) :\n");

        for (int i = 0; i < BYTE_RANGE; ++i) { //вопросик!!
            if (frequencies[i].frequency > 0) {
                printf("'%c': %ld\n", frequencies[i].byte, frequencies[i].frequency);
            }
        }

        Node* huffmanTree = buildHuffmanTree(frequencies, fileSize);
        printf("3):\n");
        printHuffmanTree(huffmanTree, 0);
        char* codes[BYTE_RANGE] = { 0 };
        char haffmancode[BYTE_RANGE] = { 0 };
        encodeFile("in.txt", "out.txt", huffmanTree, codes, haffmancode);
        decodeFile("out.txt", "decoded.txt", huffmanTree);
        // Освобождение памяти, выделенной для дерева Хаффмана
        freeHuffmanTree(huffmanTree);

        

     
        for (int i = 0; i < BYTE_RANGE; ++i) {
            free(codes[i]);
        }

        return 0;
    }