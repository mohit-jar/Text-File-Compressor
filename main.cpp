#include "huffman.h"

int main() {
    int choice;
    std::cout << "1. Compress\n2. Decompress\nEnter choice: ";
    std::cin >> choice;

    if (choice == 1) {
        std::string filename;
        std::cout << "Enter filename: ";
        std::cin >> filename;

        auto freqMap = countChar(filename);
        auto chars = buildChar(freqMap);
        auto freqs = buildFreq(freqMap);

        Node* root = buildHuffmanTree(chars, freqs);

        std::unordered_map<char, std::string> codes;
        generateCodes(root, "", codes);

        std::cout << "Huffman Codes:\n";
        for (auto &p : codes) {
            std::cout << p.first << " : " << p.second << "\n";
        }

        compressFile(filename, "compressed.bin", codes);
        std::cout << "File compressed successfully.\n";

        delete root; // free the tree
    }
    else if (choice == 2) {
        std::string filename;
        std::cout << "Enter compressed filename: ";
        std::cin >> filename;

        decompressFile(filename, "decompressed.txt");
        std::cout << "File decompressed successfully.\n";
    }
    else {
        std::cout << "Invalid choice.\n";
    }

    return 0;
}
