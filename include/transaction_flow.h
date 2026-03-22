#ifndef TRANSACTION_FLOW_H
#define TRANSACTION_FLOW_H

#include "common.h"

struct Transaction {
    string transactionId;
    string accountId;
    TransactionType type;
    double amount;
    string description;
    time_t createTime;
    bool isDeleted;
    string operatorId;

    Transaction() : amount(0.0), createTime(0), isDeleted(false) {}
};

struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode *left, *right;

    HuffmanNode(char d, int f) : data(d), frequency(f), left(nullptr), right(nullptr) {}
};

struct HuffmanCode {
    char data;
    string code;
};

class HuffmanTree {
private:
    HuffmanNode* root;
    map<char, string> codeTable;
    map<string, char> reverseCodeTable;

    void buildCodeTable(HuffmanNode* node, string currentCode);
    void destroyTree(HuffmanNode* node);

public:
    HuffmanTree();
    ~HuffmanTree();

    void build(const string& data);
    string encode(const string& data);
    string decode(const string& encodedData);
    void getCodeTable(map<char, string>& table) const;
    double getCompressionRate(const string& originalData) const;
};

class TransactionFlowModule {
private:
    map<string, Transaction> transactions;
    HuffmanTree huffmanTree;
    size_t maxTransactionSize;
    string compressedData;
    bool isCompressed;

    string transactionToString(const Transaction& trans) const;
    Transaction stringToTransaction(const string& str) const;

public:
    TransactionFlowModule(size_t maxSize = 1024);

    // CRUD operations
    bool addTransaction(const Transaction& trans);
    bool deleteTransaction(const string& transactionId);
    bool updateTransaction(const string& transactionId, const Transaction& newTrans);
    Transaction* getTransaction(const string& transactionId);
    const Transaction* getTransaction(const string& transactionId) const;
    vector<Transaction> getAllTransactions(bool includeDeleted = false) const;

    // Huffman compression
    bool compressTransactions();
    bool decompressTransactions();
    double getCompressionRate() const;
    bool isDataCompressed() const;

    // Statistics
    int getTransactionCount(bool includeDeleted = false) const;
    double getTotalTransactionAmount() const;
    size_t getTotalDataSize() const;

    // Sorting
    vector<Transaction> getSortedByTime(bool ascending = true) const;
    vector<Transaction> getSortedByAmount(bool ascending = false) const;

    // Query
    vector<Transaction> queryByAccountId(const string& accountId) const;
    vector<Transaction> queryByTimeRange(time_t startTime, time_t endTime) const;
    vector<Transaction> queryByAmountRange(double minAmount, double maxAmount) const;

    // Validation
    bool validateTransactionSize(const Transaction& trans) const;
};

#endif // TRANSACTION_FLOW_H