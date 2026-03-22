#include "transaction_module.h"
#include <queue>
#include <bitset>

string Transaction::toString() const {
    ostringstream oss;
    oss << transId << "|"
        << accountId << "|"
        << static_cast<int>(type) << "|"
        << amount << "|"
        << balanceAfter << "|"
        << transTime.toString() << "|"
        << description << "|"
        << static_cast<int>(status) << "|"
        << relatedAccount;
    return oss.str();
}

Transaction Transaction::fromString(const string& str) {
    Transaction t;
    vector<string> parts = splitString(str, '|');
    if (parts.size() >= 9) {
        t.transId = parts[0];
        t.accountId = parts[1];
        t.type = static_cast<TransactionType>(stoi(parts[2]));
        t.amount = stod(parts[3]);
        t.balanceAfter = stod(parts[4]);
        t.transTime = Date::fromString(parts[5]);
        t.description = parts[6];
        t.status = static_cast<TransactionStatus>(stoi(parts[7]));
        t.relatedAccount = parts[8];
    }
    return t;
}

string Transaction::toCompressString() const {
    ostringstream oss;
    oss << transId << ","
        << accountId << ","
        << static_cast<int>(type) << ","
        << amount << ","
        << balanceAfter << ","
        << transTime.toString() << ","
        << description << ","
        << static_cast<int>(status);
    return oss.str();
}

HuffmanTree::HuffmanTree() : root(nullptr) {}

HuffmanTree::~HuffmanTree() {
    clear();
}

void HuffmanTree::deleteTree(HuffmanNode* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

void HuffmanTree::clear() {
    deleteTree(root);
    root = nullptr;
    codeTable.clear();
    decodeTable.clear();
}

void HuffmanTree::buildTree(const string& data) {
    clear();
    
    if (data.empty()) return;
    
    map<char, int> freqMap;
    for (char c : data) {
        freqMap[c]++;
    }
    
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    
    root = pq.top();
    generateCodes(root, "");
}

void HuffmanTree::generateCodes(HuffmanNode* node, string code) {
    if (!node) return;
    
    if (node->isLeaf()) {
        if (code.empty()) code = "0";
        codeTable[node->ch] = code;
        decodeTable[code] = node->ch;
        return;
    }
    
    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

pair<string, map<char, string>> HuffmanTree::compress(const string& data) {
    buildTree(data);
    
    string compressed;
    for (char c : data) {
        if (codeTable.find(c) != codeTable.end()) {
            compressed += codeTable[c];
        }
    }
    
    return {compressed, codeTable};
}

string HuffmanTree::decompress(const string& compressed, const map<char, string>& codes) {
    if (compressed.empty()) return "";
    
    map<string, char> revCodes;
    for (const auto& pair : codes) {
        revCodes[pair.second] = pair.first;
    }
    
    string result;
    string currentCode;
    
    for (char bit : compressed) {
        currentCode += bit;
        if (revCodes.find(currentCode) != revCodes.end()) {
            result += revCodes[currentCode];
            currentCode.clear();
        }
    }
    
    return result;
}

double HuffmanTree::getCompressionRatio(const string& original, const string& compressed) {
    if (original.empty()) return 0.0;
    
    double originalBits = original.size() * 8.0;
    double compressedBits = compressed.size();
    
    return (1.0 - compressedBits / originalBits) * 100.0;
}

TransactionManager::TransactionManager()
    : transactionFile("data/transactions.txt"),
      compressedFile("data/transactions_compressed.txt") {
    loadTransactions();
}

TransactionManager::~TransactionManager() {
    saveTransactions();
}

string TransactionManager::addTransaction(const string& accountId, 
                                          TransactionType type,
                                          double amount, 
                                          double balanceAfter,
                                          const string& description,
                                          const string& relatedAccount) {
    Transaction t;
    t.transId = generateId("TXN");
    t.accountId = accountId;
    t.type = type;
    t.amount = amount;
    t.balanceAfter = balanceAfter;
    t.transTime = Date::now();
    t.description = description;
    t.status = TransactionStatus::NORMAL;
    t.relatedAccount = relatedAccount;
    
    if (t.toCompressString().size() > MAX_TRANSACTION_SIZE) {
        cout << "交易记录超过大小限制！" << endl;
        return "";
    }
    
    transactions.push_back(t);
    saveTransactions();
    
    return t.transId;
}

bool TransactionManager::deleteTransaction(const string& transId) {
    for (auto& t : transactions) {
        if (t.transId == transId && t.status == TransactionStatus::NORMAL) {
            t.status = TransactionStatus::DELETED;
            saveTransactions();
            return true;
        }
    }
    return false;
}

bool TransactionManager::updateTransaction(const string& transId, const string& newDescription) {
    for (auto& t : transactions) {
        if (t.transId == transId && t.status == TransactionStatus::NORMAL) {
            t.description = newDescription;
            saveTransactions();
            return true;
        }
    }
    return false;
}

Transaction* TransactionManager::getTransaction(const string& transId) {
    for (auto& t : transactions) {
        if (t.transId == transId) {
            return &t;
        }
    }
    return nullptr;
}

vector<Transaction> TransactionManager::getAccountTransactions(const string& accountId) {
    vector<Transaction> result;
    for (const auto& t : transactions) {
        if (t.accountId == accountId && t.status == TransactionStatus::NORMAL) {
            result.push_back(t);
        }
    }
    return result;
}

vector<Transaction> TransactionManager::getAllTransactions(SortOrder order, const string& sortBy) {
    vector<Transaction> result;
    for (const auto& t : transactions) {
        if (t.status == TransactionStatus::NORMAL) {
            result.push_back(t);
        }
    }
    
    if (sortBy == "amount") {
        sort(result.begin(), result.end(), 
             [order](const Transaction& a, const Transaction& b) -> bool {
                 if (order == SortOrder::ASC) return a.amount < b.amount;
                 return a.amount > b.amount;
             });
    } else {
        sort(result.begin(), result.end(), 
             [order](const Transaction& a, const Transaction& b) -> bool {
                 if (order == SortOrder::ASC) return a.transTime < b.transTime;
                 return a.transTime > b.transTime;
             });
    }
    
    return result;
}

bool TransactionManager::compressTransactions() {
    string allData;
    for (const auto& t : transactions) {
        if (t.status == TransactionStatus::NORMAL) {
            allData += t.toCompressString() + "\n";
        }
    }
    
    if (allData.empty()) {
        cout << "没有可压缩的数据！" << endl;
        return false;
    }
    
    auto [compressed, codes] = huffman.compress(allData);
    
    ofstream file(compressedFile);
    if (!file.is_open()) {
        return false;
    }
    
    file << "===CODES===\n";
    for (const auto& pair : codes) {
        file << static_cast<int>(static_cast<unsigned char>(pair.first)) << ":" << pair.second << "\n";
    }
    file << "===DATA===\n";
    file << compressed << "\n";
    file.close();
    
    double ratio = huffman.getCompressionRatio(allData, compressed);
    cout << "\n压缩完成！" << endl;
    cout << "原始大小: " << allData.size() << " 字节" << endl;
    cout << "压缩后大小: " << compressed.size() / 8 << " 字节" << endl;
    cout << "压缩率: " << fixed << setprecision(2) << ratio << "%" << endl;
    
    return true;
}

bool TransactionManager::decompressTransactions() {
    ifstream file(compressedFile);
    if (!file.is_open()) {
        cout << "压缩文件不存在！" << endl;
        return false;
    }
    
    string line;
    map<char, string> codes;
    string compressed;
    bool readingCodes = false;
    bool readingData = false;
    
    while (getline(file, line)) {
        if (line == "===CODES===") {
            readingCodes = true;
            continue;
        }
        if (line == "===DATA===") {
            readingCodes = false;
            readingData = true;
            continue;
        }
        
        if (readingCodes) {
            size_t pos = line.find(':');
            if (pos != string::npos) {
                int ch = stoi(line.substr(0, pos));
                string code = line.substr(pos + 1);
                codes[static_cast<char>(ch)] = code;
            }
        }
        
        if (readingData) {
            compressed += line;
        }
    }
    
    file.close();
    
    string decompressed = huffman.decompress(compressed, codes);
    
    cout << "\n解压完成！" << endl;
    cout << "解压后数据预览:" << endl;
    cout << "----------------------------------------" << endl;
    
    vector<string> lines = splitString(decompressed, '\n');
    int count = 0;
    for (const auto& l : lines) {
        if (!l.empty() && count < 5) {
            cout << l << endl;
            count++;
        }
    }
    if (lines.size() > 5) {
        cout << "... 共 " << lines.size() << " 条记录" << endl;
    }
    
    return true;
}

void TransactionManager::showCompressionStats() {
    string allData;
    int normalCount = 0;
    
    for (const auto& t : transactions) {
        if (t.status == TransactionStatus::NORMAL) {
            allData += t.toCompressString() + "\n";
            normalCount++;
        }
    }
    
    if (allData.empty()) {
        cout << "没有交易数据！" << endl;
        return;
    }
    
    auto [compressed, codes] = huffman.compress(allData);
    double ratio = huffman.getCompressionRatio(allData, compressed);
    
    cout << "\n========== 压缩统计 ==========" << endl;
    cout << "交易记录数: " << normalCount << endl;
    cout << "原始数据大小: " << allData.size() << " 字节" << endl;
    cout << "压缩后大小: " << compressed.size() / 8 << " 字节" << endl;
    cout << "压缩率: " << fixed << setprecision(2) << ratio << "%" << endl;
    cout << "编码表大小: " << codes.size() << " 个字符" << endl;
}

bool TransactionManager::loadTransactions() {
    ifstream file(transactionFile);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        Transaction t = Transaction::fromString(line);
        transactions.push_back(t);
    }
    
    file.close();
    return true;
}

bool TransactionManager::saveTransactions() {
    ofstream file(transactionFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& t : transactions) {
        file << t.toString() << "\n";
    }
    
    file.close();
    return true;
}

void TransactionManager::displayTransaction(const string& transId) {
    Transaction* t = getTransaction(transId);
    if (!t) {
        cout << "交易记录不存在！" << endl;
        return;
    }
    
    string typeStr;
    switch (t->type) {
        case TransactionType::DEPOSIT: typeStr = "存款"; break;
        case TransactionType::WITHDRAW: typeStr = "取款"; break;
        case TransactionType::TRANSFER_IN: typeStr = "转入"; break;
        case TransactionType::TRANSFER_OUT: typeStr = "转出"; break;
    }
    
    cout << "\n========== 交易详情 ==========" << endl;
    cout << "交易ID: " << t->transId << endl;
    cout << "账户ID: " << t->accountId << endl;
    cout << "交易类型: " << typeStr << endl;
    cout << "交易金额: " << fixed << setprecision(2) << t->amount << endl;
    cout << "交易后余额: " << t->balanceAfter << endl;
    cout << "交易时间: " << t->transTime.toString() << endl;
    cout << "描述: " << t->description << endl;
    cout << "状态: " << (t->status == TransactionStatus::NORMAL ? "正常" : "已删除") << endl;
    if (!t->relatedAccount.empty()) {
        cout << "关联账户: " << t->relatedAccount << endl;
    }
}

void TransactionManager::displayAccountTransactions(const string& accountId) {
    cout << "\n========== 账户交易记录 ==========" << endl;
    cout << "账户ID: " << accountId << endl;
    
    vector<Transaction> accTrans = getAccountTransactions(accountId);
    if (accTrans.empty()) {
        cout << "暂无交易记录" << endl;
        return;
    }
    
    cout << left << setw(12) << "交易ID"
         << setw(10) << "类型"
         << setw(15) << "金额"
         << setw(15) << "余额"
         << setw(20) << "时间" << endl;
    cout << string(72, '-') << endl;
    
    for (const auto& t : accTrans) {
        string typeStr;
        switch (t.type) {
            case TransactionType::DEPOSIT: typeStr = "存款"; break;
            case TransactionType::WITHDRAW: typeStr = "取款"; break;
            case TransactionType::TRANSFER_IN: typeStr = "转入"; break;
            case TransactionType::TRANSFER_OUT: typeStr = "转出"; break;
        }
        
        cout << left << setw(12) << t.transId
             << setw(10) << typeStr
             << setw(15) << fixed << setprecision(2) << t.amount
             << setw(15) << t.balanceAfter
             << setw(20) << t.transTime.toString() << endl;
    }
}

void TransactionManager::displayAllTransactions() {
    cout << "\n========== 所有交易记录 ==========" << endl;
    
    vector<Transaction> allTrans = getAllTransactions();
    if (allTrans.empty()) {
        cout << "暂无交易记录" << endl;
        return;
    }
    
    cout << left << setw(12) << "交易ID"
         << setw(12) << "账户ID"
         << setw(8) << "类型"
         << setw(12) << "金额"
         << setw(20) << "时间" << endl;
    cout << string(64, '-') << endl;
    
    for (const auto& t : allTrans) {
        string typeStr;
        switch (t.type) {
            case TransactionType::DEPOSIT: typeStr = "存款"; break;
            case TransactionType::WITHDRAW: typeStr = "取款"; break;
            case TransactionType::TRANSFER_IN: typeStr = "转入"; break;
            case TransactionType::TRANSFER_OUT: typeStr = "转出"; break;
        }
        
        cout << left << setw(12) << t.transId
             << setw(12) << t.accountId
             << setw(8) << typeStr
             << setw(12) << fixed << setprecision(2) << t.amount
             << setw(20) << t.transTime.toString() << endl;
    }
}

void TransactionManager::displayStatistics() {
    cout << "\n========== 交易统计 ==========" << endl;
    
    int deposit = 0, withdraw = 0, transferIn = 0, transferOut = 0;
    double totalDeposit = 0, totalWithdraw = 0;
    int normalCount = 0, deletedCount = 0;
    
    for (const auto& t : transactions) {
        if (t.status == TransactionStatus::NORMAL) {
            normalCount++;
            switch (t.type) {
                case TransactionType::DEPOSIT:
                    deposit++;
                    totalDeposit += t.amount;
                    break;
                case TransactionType::WITHDRAW:
                    withdraw++;
                    totalWithdraw += t.amount;
                    break;
                case TransactionType::TRANSFER_IN:
                    transferIn++;
                    break;
                case TransactionType::TRANSFER_OUT:
                    transferOut++;
                    break;
            }
        } else {
            deletedCount++;
        }
    }
    
    cout << "正常记录: " << normalCount << endl;
    cout << "已删除记录: " << deletedCount << endl;
    cout << "存款次数: " << deposit << " (总额: " << fixed << setprecision(2) 
         << totalDeposit << ")" << endl;
    cout << "取款次数: " << withdraw << " (总额: " << totalWithdraw << ")" << endl;
    cout << "转入次数: " << transferIn << endl;
    cout << "转出次数: " << transferOut << endl;
}
