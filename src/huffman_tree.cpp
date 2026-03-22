#include "../include/huffman_tree.h"
#include <queue>
#include <cstring>

string Transaction::toString() const {
    stringstream ss;
    ss << id << "|" << accountId << "|" << type << "|" 
       << amount << "|" << timestamp << "|" << description << "|" << deleted;
    return ss.str();
}

HuffmanSystem::HuffmanSystem(int cap) {
    capacity = cap;
    transactions = new Transaction[capacity];
    count = 0;
    nextId = 1;
    huffmanRoot = nullptr;
}

HuffmanSystem::~HuffmanSystem() {
    delete[] transactions;
    deleteTree(huffmanRoot);
}

void HuffmanSystem::deleteTree(HuffmanNode* root) {
    if (root == nullptr) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

HuffmanNode* HuffmanSystem::buildHuffmanTree(const int freq[]) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            pq.push(new HuffmanNode((char)i, freq[i]));
        }
    }
    
    if (pq.empty()) return nullptr;
    
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        
        HuffmanNode* parent = new HuffmanNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        
        pq.push(parent);
    }
    
    return pq.top();
}

void HuffmanSystem::generateCodes(HuffmanNode* root, string code) {
    if (root == nullptr) return;
    
    if (root->left == nullptr && root->right == nullptr) {
        huffmanCodes[(unsigned char)root->data] = code.empty() ? "0" : code;
        return;
    }
    
    generateCodes(root->left, code + "0");
    generateCodes(root->right, code + "1");
}

string HuffmanSystem::compressData(const string& data) {
    if (huffmanRoot == nullptr) return "";
    
    string compressed;
    for (char c : data) {
        compressed += huffmanCodes[(unsigned char)c];
    }
    return compressed;
}

string HuffmanSystem::decompressData(const string& data, HuffmanNode* root) {
    if (root == nullptr) return "";
    
    string decompressed;
    HuffmanNode* current = root;
    
    for (char bit : data) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        
        if (current->left == nullptr && current->right == nullptr) {
            decompressed += current->data;
            current = root;
        }
    }
    
    return decompressed;
}

int HuffmanSystem::addTransaction(int accountId, const string& type, double amount, const string& desc) {
    if (count >= capacity) {
        cout << "交易记录已满！" << endl;
        return -1;
    }
    
    transactions[count] = Transaction(nextId, accountId, type, amount, desc);
    count++;
    
    cout << "交易添加成功！ID: " << nextId << endl;
    return nextId++;
}

bool HuffmanSystem::deleteTransaction(int id) {
    for (int i = 0; i < count; i++) {
        if (transactions[i].id == id && !transactions[i].deleted) {
            transactions[i].deleted = true;
            cout << "交易 " << id << " 已标记删除！" << endl;
            return true;
        }
    }
    cout << "未找到交易 " << id << endl;
    return false;
}

bool HuffmanSystem::updateTransaction(int id, const string& field, const string& value) {
    for (int i = 0; i < count; i++) {
        if (transactions[i].id == id && !transactions[i].deleted) {
            if (field == "description") {
                transactions[i].description = value;
            } else if (field == "type") {
                transactions[i].type = value;
            } else {
                cout << "未知字段: " << field << endl;
                return false;
            }
            cout << "交易 " << id << " 更新成功！" << endl;
            return true;
        }
    }
    cout << "未找到交易 " << id << endl;
    return false;
}

Transaction* HuffmanSystem::getTransaction(int id) {
    for (int i = 0; i < count; i++) {
        if (transactions[i].id == id) {
            return &transactions[i];
        }
    }
    return nullptr;
}

void HuffmanSystem::queryTransactions(int accountId) {
    cout << "\n========== 交易流水查询 ==========" << endl;
    
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (!transactions[i].deleted && (accountId == -1 || transactions[i].accountId == accountId)) {
            cout << "  ID: " << transactions[i].id
                 << ", 账户: " << transactions[i].accountId
                 << ", 类型: " << transactions[i].type
                 << ", 金额: " << transactions[i].amount
                 << ", 时间: " << Utils::timestampToString(transactions[i].timestamp)
                 << ", 描述: " << transactions[i].description;
            if (transactions[i].deleted) {
                cout << " [已删除]";
            }
            cout << endl;
            found = true;
        }
    }
    
    if (!found) {
        cout << "  未找到交易记录" << endl;
    }
    cout << "==================================\n" << endl;
}

void HuffmanSystem::sortByTime() {
    cout << "\n========== 按交易时间排序 ==========" << endl;
    
    Transaction* sorted = new Transaction[count];
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (!transactions[i].deleted) {
            sorted[idx++] = transactions[i];
        }
    }
    
    // 冒泡排序
    for (int i = 0; i < idx - 1; i++) {
        for (int j = 0; j < idx - i - 1; j++) {
            if (sorted[j].timestamp > sorted[j + 1].timestamp) {
                Transaction temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < idx; i++) {
        cout << "  ID: " << sorted[i].id
             << ", 账户: " << sorted[i].accountId
             << ", 类型: " << sorted[i].type
             << ", 金额: " << sorted[i].amount
             << ", 时间: " << Utils::timestampToString(sorted[i].timestamp) << endl;
    }
    
    delete[] sorted;
    cout << "====================================\n" << endl;
}

void HuffmanSystem::sortByAmount() {
    cout << "\n========== 按交易金额排序 ==========" << endl;
    
    Transaction* sorted = new Transaction[count];
    int idx = 0;
    for (int i = 0; i < count; i++) {
        if (!transactions[i].deleted) {
            sorted[idx++] = transactions[i];
        }
    }
    
    // 冒泡排序
    for (int i = 0; i < idx - 1; i++) {
        for (int j = 0; j < idx - i - 1; j++) {
            if (sorted[j].amount < sorted[j + 1].amount) {
                Transaction temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < idx; i++) {
        cout << "  ID: " << sorted[i].id
             << ", 账户: " << sorted[i].accountId
             << ", 类型: " << sorted[i].type
             << ", 金额: " << sorted[i].amount
             << ", 时间: " << Utils::timestampToString(sorted[i].timestamp) << endl;
    }
    
    delete[] sorted;
    cout << "====================================\n" << endl;
}

bool HuffmanSystem::compressTransactions(const string& filename) {
    // 构建所有交易数据的字符串
    string allData;
    for (int i = 0; i < count; i++) {
        allData += transactions[i].toString() + "\n";
    }
    
    // 统计频率
    int freq[256] = {0};
    for (char c : allData) {
        freq[(unsigned char)c]++;
    }
    
    // 删除旧的哈夫曼树
    deleteTree(huffmanRoot);
    
    // 构建哈夫曼树
    huffmanRoot = buildHuffmanTree(freq);
    
    // 生成编码
    for (int i = 0; i < 256; i++) {
        huffmanCodes[i] = "";
    }
    generateCodes(huffmanRoot, "");
    
    // 压缩数据
    string compressed = compressData(allData);
    
    // 保存到文件
    ofstream file(filename, ios::binary);
    if (!file.is_open()) return false;
    
    // 保存频率表
    for (int i = 0; i < 256; i++) {
        file.write((char*)&freq[i], sizeof(int));
    }
    
    // 保存压缩数据长度和压缩数据
    size_t compressedLen = compressed.length();
    file.write((char*)&compressedLen, sizeof(size_t));
    file.write(compressed.c_str(), compressedLen);
    
    file.close();
    
    double ratio = getCompressionRatio(allData, compressed);
    cout << "压缩完成！压缩率: " << fixed << setprecision(2) << ratio << "%" << endl;
    
    return true;
}

bool HuffmanSystem::decompressTransactions(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;
    
    // 读取频率表
    int freq[256];
    for (int i = 0; i < 256; i++) {
        file.read((char*)&freq[i], sizeof(int));
    }
    
    // 读取压缩数据
    size_t compressedLen;
    file.read((char*)&compressedLen, sizeof(size_t));
    
    char* compressedData = new char[compressedLen + 1];
    file.read(compressedData, compressedLen);
    compressedData[compressedLen] = '\0';
    
    file.close();
    
    // 重建哈夫曼树
    deleteTree(huffmanRoot);
    huffmanRoot = buildHuffmanTree(freq);
    
    // 解压数据
    string decompressed = decompressData(string(compressedData), huffmanRoot);
    delete[] compressedData;
    
    cout << "解压完成！原始数据长度: " << decompressed.length() << " 字节" << endl;
    cout << "前200字符预览:\n" << decompressed.substr(0, 200) << "..." << endl;
    
    return true;
}

double HuffmanSystem::getCompressionRatio(const string& original, const string& compressed) {
    if (original.empty()) return 0.0;
    double originalBits = original.length() * 8.0;
    double compressedBits = compressed.length();
    return (1.0 - compressedBits / originalBits) * 100.0;
}

void HuffmanSystem::showCompressionStats() {
    cout << "\n========== 压缩统计 ==========" << endl;
    
    if (huffmanRoot == nullptr) {
        cout << "尚未进行压缩操作！" << endl;
        return;
    }
    
    cout << "哈夫曼编码表(前20个):" << endl;
    int shown = 0;
    for (int i = 0; i < 256 && shown < 20; i++) {
        if (!huffmanCodes[i].empty()) {
            cout << "  '" << (char)i << "': " << huffmanCodes[i] << endl;
            shown++;
        }
    }
    
    cout << "==============================\n" << endl;
}

bool HuffmanSystem::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << count << " " << nextId << endl;
    
    for (int i = 0; i < count; i++) {
        file << transactions[i].id << " "
             << transactions[i].accountId << " "
             << transactions[i].type << " "
             << transactions[i].amount << " "
             << transactions[i].timestamp << " "
             << transactions[i].deleted << " "
             << transactions[i].description << endl;
    }
    
    file.close();
    return true;
}

bool HuffmanSystem::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> count >> nextId;
    
    for (int i = 0; i < count; i++) {
        file >> transactions[i].id
             >> transactions[i].accountId
             >> transactions[i].type
             >> transactions[i].amount
             >> transactions[i].timestamp
             >> transactions[i].deleted;
        file.ignore();
        getline(file, transactions[i].description);
    }
    
    file.close();
    return true;
}

bool HuffmanSystem::exportToText(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    for (int i = 0; i < count; i++) {
        if (!transactions[i].deleted) {
            file << transactions[i].toString() << endl;
        }
    }
    
    file.close();
    cout << "导出成功！文件名: " << filename << endl;
    return true;
}

void HuffmanSystem::showMenu() {
    cout << "\n========== 交易流水管理系统 ==========" << endl;
    cout << "1. 添加交易" << endl;
    cout << "2. 删除交易(标记删除)" << endl;
    cout << "3. 更新交易" << endl;
    cout << "4. 查询交易" << endl;
    cout << "5. 按时间排序" << endl;
    cout << "6. 按金额排序" << endl;
    cout << "7. 哈夫曼压缩" << endl;
    cout << "8. 哈夫曼解压" << endl;
    cout << "9. 压缩统计" << endl;
    cout << "10. 导出文本" << endl;
    cout << "11. 保存数据" << endl;
    cout << "12. 加载数据" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "======================================" << endl;
    cout << "请选择: ";
}

void HuffmanSystem::run() {
    int choice;
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                int accountId;
                string type, desc;
                double amount;
                cout << "请输入账户ID: ";
                cin >> accountId;
                cout << "请输入交易类型(存款/取款/转账): ";
                cin >> type;
                cout << "请输入金额: ";
                cin >> amount;
                cout << "请输入描述: ";
                cin.ignore();
                getline(cin, desc);
                addTransaction(accountId, type, amount, desc);
                break;
            }
            case 2: {
                int id;
                cout << "请输入要删除的交易ID: ";
                cin >> id;
                deleteTransaction(id);
                break;
            }
            case 3: {
                int id;
                string field, value;
                cout << "请输入交易ID: ";
                cin >> id;
                cout << "请输入要更新的字段(description/type): ";
                cin >> field;
                cout << "请输入新值: ";
                cin.ignore();
                getline(cin, value);
                updateTransaction(id, field, value);
                break;
            }
            case 4: {
                int accountId;
                cout << "请输入账户ID(-1查看全部): ";
                cin >> accountId;
                queryTransactions(accountId);
                break;
            }
            case 5: {
                sortByTime();
                break;
            }
            case 6: {
                sortByAmount();
                break;
            }
            case 7: {
                if (compressTransactions("transactions.huf")) {
                    cout << "压缩成功！" << endl;
                } else {
                    cout << "压缩失败！" << endl;
                }
                break;
            }
            case 8: {
                if (decompressTransactions("transactions.huf")) {
                    cout << "解压成功！" << endl;
                } else {
                    cout << "解压失败！" << endl;
                }
                break;
            }
            case 9: {
                showCompressionStats();
                break;
            }
            case 10: {
                exportToText("transactions_export.txt");
                break;
            }
            case 11: {
                if (saveToFile("transaction_data.txt")) {
                    cout << "数据保存成功！" << endl;
                } else {
                    cout << "数据保存失败！" << endl;
                }
                break;
            }
            case 12: {
                if (loadFromFile("transaction_data.txt")) {
                    cout << "数据加载成功！" << endl;
                } else {
                    cout << "数据加载失败！" << endl;
                }
                break;
            }
            case 0: {
                cout << "返回主菜单..." << endl;
                break;
            }
            default: {
                cout << "无效选择，请重试！" << endl;
            }
        }
    } while (choice != 0);
}
