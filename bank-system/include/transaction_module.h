#ifndef TRANSACTION_MODULE_H
#define TRANSACTION_MODULE_H

#include "common.h"

enum class TransactionType {
    DEPOSIT,
    WITHDRAW,
    TRANSFER_IN,
    TRANSFER_OUT
};

enum class TransactionStatus {
    NORMAL,
    DELETED
};

struct Transaction {
    string transId;
    string accountId;
    TransactionType type;
    double amount;
    double balanceAfter;
    Date transTime;
    string description;
    TransactionStatus status;
    string relatedAccount;
    
    Transaction() : type(TransactionType::DEPOSIT), amount(0), 
                    balanceAfter(0), status(TransactionStatus::NORMAL) {}
    
    string toString() const;
    static Transaction fromString(const string& str);
    string toCompressString() const;
    
    bool operator<(const Transaction& other) const {
        return transTime < other.transTime;
    }
};

struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char c = '\0', int f = 0) 
        : ch(c), freq(f), left(nullptr), right(nullptr) {}
    
    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

class HuffmanTree {
private:
    HuffmanNode* root;
    map<char, string> codeTable;
    map<string, char> decodeTable;
    
    void buildTree(const string& data);
    void generateCodes(HuffmanNode* node, string code);
    void deleteTree(HuffmanNode* node);
    
public:
    HuffmanTree();
    ~HuffmanTree();
    
    pair<string, map<char, string> > compress(const string& data);
    string decompress(const string& compressed, const map<char, string>& codes);
    double getCompressionRatio(const string& original, const string& compressed);
    void clear();
};

class TransactionManager {
private:
    vector<Transaction> transactions;
    HuffmanTree huffman;
    
    string transactionFile;
    string compressedFile;
    
    static const size_t MAX_TRANSACTION_SIZE = 1024;
    
public:
    TransactionManager();
    ~TransactionManager();
    
    string addTransaction(const string& accountId, TransactionType type, 
                          double amount, double balanceAfter,
                          const string& description = "",
                          const string& relatedAccount = "");
    bool deleteTransaction(const string& transId);
    bool updateTransaction(const string& transId, const string& newDescription);
    
    Transaction* getTransaction(const string& transId);
    vector<Transaction> getAccountTransactions(const string& accountId);
    vector<Transaction> getAllTransactions(SortOrder order = SortOrder::ASC,
                                            const string& sortBy = "time");
    
    bool compressTransactions();
    bool decompressTransactions();
    void showCompressionStats();
    
    bool loadTransactions();
    bool saveTransactions();
    
    void displayTransaction(const string& transId);
    void displayAccountTransactions(const string& accountId);
    void displayAllTransactions();
    void displayStatistics();
};

#endif
