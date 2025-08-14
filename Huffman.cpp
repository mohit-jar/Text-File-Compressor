#include "huffman.h"
#include <bitset>

// Node constructor
Node::Node(char c, int f, bool leaf) : character(c), frequency(f), left(nullptr), right(nullptr), isLeaf(leaf) {}

// Destructor recursively deletes children
Node::~Node() {
    delete left;
    delete right;
}

// Comparator for min-heap
bool Compare::operator()(Node* a, Node* b) {
    return a->frequency > b->frequency;
}

// Count character frequencies
std::unordered_map<char, int> countChar(const std::string &filename) {
    std::unordered_map<char, int> charCount;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: File not found\n";
        return charCount;
    }
    char c;
    while (file.get(c)) {
        charCount[c]++;
    }
    return charCount;
}

std::vector<char> buildChar(const std::unordered_map<char, int> &map) {
    std::vector<char> chars;
    for (auto &p : map) chars.push_back(p.first);
    return chars;
}

std::vector<int> buildFreq(const std::unordered_map<char, int> &map) {
    std::vector<int> freqs;
    for (auto &p : map) freqs.push_back(p.second);
    return freqs;
}

// Build Huffman tree
Node* buildHuffmanTree(const std::vector<char> &chars, const std::vector<int> &freq) {
    std::priority_queue<Node*, std::vector<Node*>, Compare> minHeap;

    for (size_t i = 0; i < chars.size(); i++) {
        minHeap.push(new Node(chars[i], freq[i], true)); // leaf node
    }

    while (minHeap.size() > 1) {
        Node* left = minHeap.top(); minHeap.pop();
        Node* right = minHeap.top(); minHeap.pop();

        Node* merged = new Node('\0', left->frequency + right->frequency, false); // internal node
        merged->left = left;
        merged->right = right;

        minHeap.push(merged);
    }

    return minHeap.top();
}

// Generate Huffman codes
void generateCodes(Node* root, std::string code, std::unordered_map<char, std::string> &codes) {
    if (!root) return;

    if (root->isLeaf) {
        codes[root->character] = code;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Compress file
void compressFile(const std::string &inputFile, const std::string &outputFile,
                  const std::unordered_map<char, std::string> &codes) {
    std::ifstream in(inputFile);
    std::ofstream out(outputFile, std::ios::binary);

    size_t count = codes.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    // Write Huffman codes
    for (auto &p : codes) {
        out.put(p.first);
        unsigned char len = p.second.size();
        out.put(len);
        out << p.second;
    }
    out.put('\n');

    // Encode file
    std::string bits;
    char c;
    while (in.get(c)) {
        bits += codes.at(c);
    }

    size_t padding = (8 - bits.size() % 8) % 8;
    bits.append(padding, '0');

    size_t bitSize = bits.size();
    out.write(reinterpret_cast<const char*>(&bitSize), sizeof(bitSize));

    for (size_t i = 0; i < bits.size(); i += 8) {
        std::bitset<8> byte(bits.substr(i, 8));
        out.put(static_cast<unsigned char>(byte.to_ulong()));
    }
}

// Decompress file
void decompressFile(const std::string &inputFile, const std::string &outputFile) {
    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile);

    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    std::unordered_map<std::string, char> reverseCodes;
    for (size_t i = 0; i < count; i++) {
        char ch = in.get();
        unsigned char len = in.get();
        std::string code;
        code.resize(len);
        in.read(&code[0], len);
        reverseCodes[code] = ch;
    }
    in.get(); // skip '\n'

    size_t bitSize;
    in.read(reinterpret_cast<char*>(&bitSize), sizeof(bitSize));

    std::string bits;
    char byte;
    while (in.get(byte)) {
        bits += std::bitset<8>(static_cast<unsigned char>(byte)).to_string();
    }
    bits.resize(bitSize);

    std::string current;
    for (char b : bits) {
        current.push_back(b);
        if (reverseCodes.count(current)) {
            out.put(reverseCodes[current]);
            current.clear();
        }
    }
}
