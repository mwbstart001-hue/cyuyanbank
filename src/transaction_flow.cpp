#include "transaction_flow.h"

// HuffmanTree implementation
HuffmanTree::HuffmanTree() : root(nullptr) {}

HuffmanTree::~HuffmanTree() {
    destroyTree(root);
}

void HuffmanTree::destroyTree(HuffmanNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

void HuffmanTree::buildCodeTable(HuffmanNode* node, string currentCode) {
    if (!node) return;

    if (!node->left && !node->right) {
        codeTable[node->data] = currentCode;
        reverseCodeTable[currentCode] = node->data;
        return;
    }

    buildCodeTable(node->left, currentCode + "0");
    buildCodeTable(node->right, currentCode + "1");
}

void HuffmanTree::build(const string& data) {
    destroyTree(root);
    codeTable.clear();
    reverseCodeTable.clear();

    map<char, int> freq;
    for (char c : data) {
        freq[c]++;
    }

    auto compare = [](HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    };
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, decltype(compare)> pq(compare);

    for (const auto& pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* parent = new HuffmanNode('\0', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    if (!pq.empty()) {
        root = pq.top();
        buildCodeTable(root, "");
    }
}

string HuffmanTree::encode(const string& data) {
    string result;
    for (char c : data) {
        auto it = codeTable.find(c);
        if (it != codeTable.end()) {
            result += it->second;
        }
    }
    return result;
}

string HuffmanTree::decode(const string& encodedData) {
    string result;
    string currentCode;

    for (char bit : encodedData) {
        currentCode += bit;
        auto it = reverseCodeTable.find(currentCode);
        if (it != reverseCodeTable.end()) {
            result += it->second;
            currentCode.clear();
        }
    }
    return result;
}

void HuffmanTree::getCodeTable(map<char, string>& table) const {
    table = codeTable;
}

double HuffmanTree::getCompressionRate(const string& originalData) const {
    if (originalData.empty()) return 0.0;

    size_t originalBits = originalData.size() * 8;
    size_t compressedBits = 0;

    map<char, int> freq;
    for (char c : originalData) {
        freq[c]++;
    }

    for (const auto& pair : freq) {
        auto it = codeTable.find(pair.first);
        if (it != codeTable.end()) {
            compressedBits += pair.second * it->second.size();
        }
    }

    return static_cast<double>(originalBits - compressedBits) / originalBits * 100.0;
}

// TransactionFlowModule implementation
TransactionFlowModule::TransactionFlowModule(size_t maxSize)
    : maxTransactionSize(maxSize), isCompressed(false) {}

string TransactionFlowModule::transactionToString(const Transaction& trans) const {
    stringstream ss;
    ss << trans.transactionId << "|"
       << trans.accountId << "|"
       << static_cast<int>(trans.type) << "|"
       << trans.amount << "|"
       << trans.description << "|"
       << trans.createTime << "|"
       << (trans.isDeleted ? "1" : "0") << "|"
       << trans.operatorId << "||";
    return ss.str();
}

Transaction TransactionFlowModule::stringToTransaction(const string& str) const {
    Transaction trans;
    stringstream ss(str);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 8) {
        trans.transactionId = tokens[0];
        trans.accountId = tokens[1];
        trans.type = static_cast<TransactionType>(stoi(tokens[2]));
        trans.amount = stod(tokens[3]);
        trans.description = tokens[4];
        trans.createTime = stoll(tokens[5]);
        trans.isDeleted = (tokens[6] == "1");
        trans.operatorId = tokens[7];
    }
    return trans;
}

bool TransactionFlowModule::addTransaction(const Transaction& trans) {
    if (isCompressed) {
        decompressTransactions();
    }

    if (transactions.find(trans.transactionId) != transactions.end()) {
        return false;
    }

    if (!validateTransactionSize(trans)) {
        return false;
    }

    transactions[trans.transactionId] = trans;
    return true;
}

bool TransactionFlowModule::deleteTransaction(const string& transactionId) {
    if (isCompressed) {
        decompressTransactions();
    }

    auto it = transactions.find(transactionId);
    if (it == transactions.end()) {
        return false;
    }

    it->second.isDeleted = true;
    return true;
}

bool TransactionFlowModule::updateTransaction(const string& transactionId, const Transaction& newTrans) {
    if (isCompressed) {
        decompressTransactions();
    }

    auto it = transactions.find(transactionId);
    if (it == transactions.end()) {
        return false;
    }

    if (!validateTransactionSize(newTrans)) {
        return false;
    }

    transactions[transactionId] = newTrans;
    return true;
}

Transaction* TransactionFlowModule::getTransaction(const string& transactionId) {
    if (isCompressed) {
        decompressTransactions();
    }

    auto it = transactions.find(transactionId);
    if (it != transactions.end()) {
        return &it->second;
    }
    return nullptr;
}

const Transaction* TransactionFlowModule::getTransaction(const string& transactionId) const {
    auto it = transactions.find(transactionId);
    if (it != transactions.end()) {
        return &it->second;
    }
    return nullptr;
}

vector<Transaction> TransactionFlowModule::getAllTransactions(bool includeDeleted) const {
    vector<Transaction> result;
    for (const auto& pair : transactions) {
        if (includeDeleted || !pair.second.isDeleted) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool TransactionFlowModule::compressTransactions() {
    if (isCompressed || transactions.empty()) {
        return false;
    }

    string allData;
    for (const auto& pair : transactions) {
        allData += transactionToString(pair.second);
    }

    huffmanTree.build(allData);
    compressedData = huffmanTree.encode(allData);
    transactions.clear();
    isCompressed = true;
    return true;
}

bool TransactionFlowModule::decompressTransactions() {
    if (!isCompressed || compressedData.empty()) {
        return false;
    }

    string decodedData = huffmanTree.decode(compressedData);
    transactions.clear();

    size_t pos = 0;
    size_t nextPos;
    while ((nextPos = decodedData.find("||", pos)) != string::npos) {
        string transStr = decodedData.substr(pos, nextPos - pos);
        if (!transStr.empty()) {
            Transaction trans = stringToTransaction(transStr);
            if (!trans.transactionId.empty()) {
                transactions[trans.transactionId] = trans;
            }
        }
        pos = nextPos + 2;
    }

    isCompressed = false;
    return true;
}

double TransactionFlowModule::getCompressionRate() const {
    if (!isCompressed) {
        string allData;
        for (const auto& pair : transactions) {
            allData += transactionToString(pair.second);
        }
        if (allData.empty()) return 0.0;

        HuffmanTree tempTree;
        tempTree.build(allData);
        return tempTree.getCompressionRate(allData);
    }
    return 0.0;
}

bool TransactionFlowModule::isDataCompressed() const {
    return isCompressed;
}

int TransactionFlowModule::getTransactionCount(bool includeDeleted) const {
    if (isCompressed) {
        return -1;
    }

    int count = 0;
    for (const auto& pair : transactions) {
        if (includeDeleted || !pair.second.isDeleted) {
            count++;
        }
    }
    return count;
}

double TransactionFlowModule::getTotalTransactionAmount() const {
    if (isCompressed) {
        return -1;
    }

    double total = 0.0;
    for (const auto& pair : transactions) {
        if (!pair.second.isDeleted) {
            total += pair.second.amount;
        }
    }
    return total;
}

size_t TransactionFlowModule::getTotalDataSize() const {
    if (isCompressed) {
        return compressedData.size() / 8 + (compressedData.size() % 8 ? 1 : 0);
    }

    size_t total = 0;
    for (const auto& pair : transactions) {
        total += transactionToString(pair.second).size();
    }
    return total;
}

vector<Transaction> TransactionFlowModule::getSortedByTime(bool ascending) const {
    vector<Transaction> result = getAllTransactions();
    sort(result.begin(), result.end(), [ascending](const Transaction& a, const Transaction& b) {
        return ascending ? (a.createTime < b.createTime) : (a.createTime > b.createTime);
    });
    return result;
}

vector<Transaction> TransactionFlowModule::getSortedByAmount(bool ascending) const {
    vector<Transaction> result = getAllTransactions();
    sort(result.begin(), result.end(), [ascending](const Transaction& a, const Transaction& b) {
        return ascending ? (a.amount < b.amount) : (a.amount > b.amount);
    });
    return result;
}

vector<Transaction> TransactionFlowModule::queryByAccountId(const string& accountId) const {
    vector<Transaction> result;
    for (const auto& pair : transactions) {
        if (!pair.second.isDeleted && pair.second.accountId == accountId) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<Transaction> TransactionFlowModule::queryByTimeRange(time_t startTime, time_t endTime) const {
    vector<Transaction> result;
    for (const auto& pair : transactions) {
        if (!pair.second.isDeleted &&
            pair.second.createTime >= startTime &&
            pair.second.createTime <= endTime) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<Transaction> TransactionFlowModule::queryByAmountRange(double minAmount, double maxAmount) const {
    vector<Transaction> result;
    for (const auto& pair : transactions) {
        if (!pair.second.isDeleted &&
            pair.second.amount >= minAmount &&
            pair.second.amount <= maxAmount) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool TransactionFlowModule::validateTransactionSize(const Transaction& trans) const {
    return transactionToString(trans).size() <= maxTransactionSize;
}