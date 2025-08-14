#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
using namespace std;

struct Node {
    char character;       // Valid if isLeaf == true
    int frequency;
    Node* left;
    Node* right;
    bool isLeaf;          // True if leaf, false if internal

    Node(char c, int f, bool leaf);
    ~Node(); // Destructor deletes children
};

// Comparator for min-heap
struct Compare {
    bool operator()(Node* a, Node* b);
};

// Count character frequencies from file
unordered_map<char, int> countChar(const string &filename);
vector<char> buildChar(const unordered_map<char, int> &map);
vector<int> buildFreq(const unordered_map<char, int> &map);

// Build Huffman tree and generate codes
Node* buildHuffmanTree(const vector<char> &chars, const vector<int> &freq);
void generateCodes(Node* root, string code, unordered_map<char, string> &codes);

// Compression and decompression
void compressFile(const string &inputFile, const string &outputFile,
                  const unordered_map<char, string> &codes);
void decompressFile(const string &inputFile, const string &outputFile);

#endif
