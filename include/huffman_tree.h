#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include "common.h"

// 哈夫曼树节点
struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

// 交易流水记录
struct Transaction {
    int id;
    int accountId;
    string type;
    double amount;
    long timestamp;
    string description;
    bool deleted;
    
    Transaction() : id(0), accountId(0), amount(0), timestamp(0), deleted(false) {}
    
    Transaction(int i, int acc, const string& t, double amt, const string& desc)
        : id(i), accountId(acc), type(t), amount(amt), 
          timestamp(Utils::getCurrentTimestamp()), description(desc), deleted(false) {}
    
    string toString() const;
};

// 哈夫曼树实现的交易流水压缩存储
class HuffmanSystem : public FileStorage {
private:
    Transaction* transactions;
    int capacity;
    int count;
    int nextId;
    
    HuffmanNode* huffmanRoot;
    string huffmanCodes[256];
    
    // 哈夫曼树操作
    HuffmanNode* buildHuffmanTree(const int freq[]);
    void generateCodes(HuffmanNode* root, string code);
    void deleteTree(HuffmanNode* root);
    
    // 压缩解压
    string compressData(const string& data);
    string decompressData(const string& data, HuffmanNode* root);
    
public:
    HuffmanSystem(int cap = 10000);
    ~HuffmanSystem();
    
    // 流水管理
    int addTransaction(int accountId, const string& type, double amount, const string& desc);
    bool deleteTransaction(int id);
    bool updateTransaction(int id, const string& field, const string& value);
    Transaction* getTransaction(int id);
    void queryTransactions(int accountId = -1);
    
    // 排序查询
    void sortByTime();
    void sortByAmount();
    
    // 哈夫曼压缩
    bool compressTransactions(const string& filename);
    bool decompressTransactions(const string& filename);
    double getCompressionRatio(const string& original, const string& compressed);
    void showCompressionStats();
    
    // 文件存储
    bool saveToFile(const string& filename) override;
    bool loadFromFile(const string& filename) override;
    bool exportToText(const string& filename);
    
    // 菜单接口
    void showMenu();
    void run();
};

#endif
