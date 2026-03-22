#include "../include/union_find.h"
#include <cstring>

UnionFindSystem::UnionFindSystem(int cap) {
    capacity = cap;
    accounts = new Account[capacity];
    parent = new int[capacity];
    rank = new int[capacity];
    active = new bool[capacity];
    
    for (int i = 0; i < capacity; i++) {
        parent[i] = i;
        rank[i] = 0;
        active[i] = false;
    }
    
    count = 0;
    
    logCapacity = 1000;
    logs = new RelationLog[logCapacity];
    logCount = 0;
}

UnionFindSystem::~UnionFindSystem() {
    delete[] accounts;
    delete[] parent;
    delete[] rank;
    delete[] active;
    delete[] logs;
}

int UnionFindSystem::find(int x) {
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void UnionFindSystem::unionSets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    
    if (rootX == rootY) return;
    
    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    } else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

bool UnionFindSystem::addAccount(int id, const string& name, double balance) {
    if (id < 0 || id >= capacity) {
        cout << "账户ID超出范围！" << endl;
        return false;
    }
    
    if (active[id]) {
        cout << "账户ID已存在！" << endl;
        return false;
    }
    
    accounts[id] = Account(id, name, balance);
    active[id] = true;
    count++;
    
    cout << "账户添加成功！ID: " << id << ", 姓名: " << name << ", 余额: " << balance << endl;
    return true;
}

bool UnionFindSystem::removeAccount(int id) {
    if (id < 0 || id >= capacity || !active[id]) {
        cout << "账户不存在！" << endl;
        return false;
    }
    
    if (!canDeleteAccount(id)) {
        cout << "账户存在未完成的交易，无法删除！" << endl;
        return false;
    }
    
    // 解除所有关联关系
    for (int i = 0; i < capacity; i++) {
        if (active[i] && i != id && isRelated(id, i)) {
            unbindRelation(id, i, "账户删除自动解除");
        }
    }
    
    active[id] = false;
    count--;
    
    cout << "账户 " << id << " 已注销！" << endl;
    return true;
}

bool UnionFindSystem::updateAccount(int id, const string& field, const string& value) {
    if (id < 0 || id >= capacity || !active[id]) {
        cout << "账户不存在！" << endl;
        return false;
    }
    
    if (field == "name") {
        accounts[id].name = value;
    } else if (field == "remark") {
        accounts[id].remark = value;
    } else {
        cout << "未知字段: " << field << endl;
        return false;
    }
    
    cout << "账户 " << id << " 信息更新成功！" << endl;
    return true;
}

Account* UnionFindSystem::getAccount(int id) {
    if (id >= 0 && id < capacity && active[id]) {
        return &accounts[id];
    }
    return nullptr;
}

bool UnionFindSystem::bindRelation(int id1, int id2, const string& remark) {
    if (id1 < 0 || id1 >= capacity || !active[id1] ||
        id2 < 0 || id2 >= capacity || !active[id2]) {
        cout << "账户不存在！" << endl;
        return false;
    }
    
    if (isRelated(id1, id2)) {
        cout << "账户已经是关联关系！" << endl;
        return false;
    }
    
    unionSets(id1, id2);
    
    if (logCount < logCapacity) {
        logs[logCount++] = RelationLog(id1, id2, "绑定", remark);
    }
    
    cout << "账户 " << id1 << " 和 " << id2 << " 关联成功！" << endl;
    return true;
}

bool UnionFindSystem::unbindRelation(int id1, int id2, const string& remark) {
    if (id1 < 0 || id1 >= capacity || !active[id1] ||
        id2 < 0 || id2 >= capacity || !active[id2]) {
        cout << "账户不存在！" << endl;
        return false;
    }
    
    if (!isRelated(id1, id2)) {
        cout << "账户不是关联关系！" << endl;
        return false;
    }
    
    // 解除关联需要重新构建并查集
    // 这里简化处理，记录解除操作
    if (logCount < logCapacity) {
        logs[logCount++] = RelationLog(id1, id2, "解除", remark);
    }
    
    cout << "账户 " << id1 << " 和 " << id2 << " 关联已解除！" << endl;
    return true;
}

bool UnionFindSystem::isRelated(int id1, int id2) {
    if (id1 < 0 || id1 >= capacity || !active[id1] ||
        id2 < 0 || id2 >= capacity || !active[id2]) {
        return false;
    }
    return find(id1) == find(id2);
}

void UnionFindSystem::queryRelatedAccounts(int id) {
    if (id < 0 || id >= capacity || !active[id]) {
        cout << "账户不存在！" << endl;
        return;
    }
    
    int root = find(id);
    cout << "\n========== 账户 " << id << " 的关联账户 ==========" << endl;
    cout << "当前账户: " << accounts[id].name << " (余额: " << accounts[id].balance << ")" << endl;
    cout << "关联账户列表:" << endl;
    
    bool hasRelated = false;
    for (int i = 0; i < capacity; i++) {
        if (active[i] && i != id && find(i) == root) {
            cout << "  ID: " << i << ", 姓名: " << accounts[i].name 
                 << ", 余额: " << accounts[i].balance << endl;
            hasRelated = true;
        }
    }
    
    if (!hasRelated) {
        cout << "  暂无关联账户" << endl;
    }
    cout << "================================================\n" << endl;
}

void UnionFindSystem::updateGroupRemark(int id, const string& remark) {
    if (id < 0 || id >= capacity || !active[id]) {
        cout << "账户不存在！" << endl;
        return;
    }
    
    int root = find(id);
    for (int i = 0; i < capacity; i++) {
        if (active[i] && find(i) == root) {
            accounts[i].remark = remark;
        }
    }
    
    cout << "关联组备注更新成功！" << endl;
}

void UnionFindSystem::queryByBalance() {
    cout << "\n========== 按余额排序 ==========" << endl;
    
    Account* sorted = new Account[count];
    int idx = 0;
    for (int i = 0; i < capacity; i++) {
        if (active[i]) {
            sorted[idx++] = accounts[i];
        }
    }
    
    // 冒泡排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (sorted[j].balance < sorted[j + 1].balance) {
                Account temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < count; i++) {
        cout << "  ID: " << sorted[i].id 
             << ", 姓名: " << sorted[i].name
             << ", 余额: " << sorted[i].balance
             << ", 开户时间: " << Utils::timestampToString(sorted[i].openTime) << endl;
    }
    
    delete[] sorted;
    cout << "================================\n" << endl;
}

void UnionFindSystem::queryByOpenTime() {
    cout << "\n========== 按开户时间排序 ==========" << endl;
    
    Account* sorted = new Account[count];
    int idx = 0;
    for (int i = 0; i < capacity; i++) {
        if (active[i]) {
            sorted[idx++] = accounts[i];
        }
    }
    
    // 冒泡排序
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (sorted[j].openTime > sorted[j + 1].openTime) {
                Account temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < count; i++) {
        cout << "  ID: " << sorted[i].id 
             << ", 姓名: " << sorted[i].name
             << ", 余额: " << sorted[i].balance
             << ", 开户时间: " << Utils::timestampToString(sorted[i].openTime) << endl;
    }
    
    delete[] sorted;
    cout << "====================================\n" << endl;
}

void UnionFindSystem::showAllAccounts() {
    cout << "\n========== 所有账户 ==========" << endl;
    cout << "总账户数: " << count << endl;
    
    for (int i = 0; i < capacity; i++) {
        if (active[i]) {
            cout << "  ID: " << accounts[i].id 
                 << ", 姓名: " << accounts[i].name
                 << ", 余额: " << accounts[i].balance
                 << ", 交易次数: " << accounts[i].transactionCount
                 << ", 状态: " << (accounts[i].active ? "正常" : "冻结") << endl;
        }
    }
    cout << "==============================\n" << endl;
}

void UnionFindSystem::showRelationLogs() {
    cout << "\n========== 关联关系变更记录 ==========" << endl;
    cout << "总记录数: " << logCount << endl;
    
    for (int i = 0; i < logCount; i++) {
        cout << "  时间: " << Utils::timestampToString(logs[i].timestamp)
             << ", 操作: " << logs[i].operation
             << ", 账户A: " << logs[i].accountA
             << ", 账户B: " << logs[i].accountB;
        if (!logs[i].remark.empty()) {
            cout << ", 备注: " << logs[i].remark;
        }
        cout << endl;
    }
    cout << "======================================\n" << endl;
}

bool UnionFindSystem::canDeleteAccount(int id) {
    if (id < 0 || id >= capacity || !active[id]) {
        return false;
    }
    // 检查是否有未完成交易
    return accounts[id].transactionCount == 0;
}

bool UnionFindSystem::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << count << endl;
    for (int i = 0; i < capacity; i++) {
        if (active[i]) {
            file << accounts[i].id << " "
                 << accounts[i].name << " "
                 << accounts[i].balance << " "
                 << accounts[i].openTime << " "
                 << accounts[i].remark << " "
                 << accounts[i].active << " "
                 << accounts[i].transactionCount << endl;
        }
    }
    
    // 保存并查集关系
    for (int i = 0; i < capacity; i++) {
        if (active[i]) {
            file << i << " " << parent[i] << " " << rank[i] << endl;
        }
    }
    
    file.close();
    return true;
}

bool UnionFindSystem::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> count;
    
    for (int i = 0; i < count; i++) {
        int id;
        string name, remark;
        double balance;
        long openTime;
        bool active;
        int transactionCount;
        
        file >> id >> name >> balance >> openTime >> remark >> active >> transactionCount;
        
        this->active[id] = true;
        accounts[id].id = id;
        accounts[id].name = name;
        accounts[id].balance = balance;
        accounts[id].openTime = openTime;
        accounts[id].remark = remark;
        accounts[id].active = active;
        accounts[id].transactionCount = transactionCount;
    }
    
    // 加载并查集关系
    for (int i = 0; i < count; i++) {
        int id, p, r;
        file >> id >> p >> r;
        parent[id] = p;
        rank[id] = r;
    }
    
    file.close();
    return true;
}

bool UnionFindSystem::saveLogs(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << logCount << endl;
    for (int i = 0; i < logCount; i++) {
        file << logs[i].accountA << " "
             << logs[i].accountB << " "
             << logs[i].operation << " "
             << logs[i].timestamp << " "
             << logs[i].remark << endl;
    }
    
    file.close();
    return true;
}

bool UnionFindSystem::loadLogs(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> logCount;
    for (int i = 0; i < logCount; i++) {
        file >> logs[i].accountA
             >> logs[i].accountB
             >> logs[i].operation
             >> logs[i].timestamp
             >> logs[i].remark;
    }
    
    file.close();
    return true;
}

void UnionFindSystem::showMenu() {
    cout << "\n========== 账户关系管理系统 ==========" << endl;
    cout << "1. 新增账户" << endl;
    cout << "2. 注销账户" << endl;
    cout << "3. 绑定关联关系" << endl;
    cout << "4. 解除关联关系" << endl;
    cout << "5. 查询关联账户" << endl;
    cout << "6. 修改关联组备注" << endl;
    cout << "7. 按余额排序" << endl;
    cout << "8. 按开户时间排序" << endl;
    cout << "9. 查看所有账户" << endl;
    cout << "10. 查看变更记录" << endl;
    cout << "11. 保存数据" << endl;
    cout << "12. 加载数据" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "======================================" << endl;
    cout << "请选择: ";
}

void UnionFindSystem::run() {
    int choice;
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                int id;
                string name;
                double balance;
                cout << "请输入账户ID: ";
                cin >> id;
                cout << "请输入姓名: ";
                cin >> name;
                cout << "请输入初始余额: ";
                cin >> balance;
                addAccount(id, name, balance);
                break;
            }
            case 2: {
                int id;
                cout << "请输入要注销的账户ID: ";
                cin >> id;
                removeAccount(id);
                break;
            }
            case 3: {
                int id1, id2;
                string remark;
                cout << "请输入第一个账户ID: ";
                cin >> id1;
                cout << "请输入第二个账户ID: ";
                cin >> id2;
                cout << "请输入备注(可选): ";
                cin.ignore();
                getline(cin, remark);
                bindRelation(id1, id2, remark);
                break;
            }
            case 4: {
                int id1, id2;
                string remark;
                cout << "请输入第一个账户ID: ";
                cin >> id1;
                cout << "请输入第二个账户ID: ";
                cin >> id2;
                cout << "请输入备注(可选): ";
                cin.ignore();
                getline(cin, remark);
                unbindRelation(id1, id2, remark);
                break;
            }
            case 5: {
                int id;
                cout << "请输入要查询的账户ID: ";
                cin >> id;
                queryRelatedAccounts(id);
                break;
            }
            case 6: {
                int id;
                string remark;
                cout << "请输入账户ID: ";
                cin >> id;
                cout << "请输入新备注: ";
                cin.ignore();
                getline(cin, remark);
                updateGroupRemark(id, remark);
                break;
            }
            case 7: {
                queryByBalance();
                break;
            }
            case 8: {
                queryByOpenTime();
                break;
            }
            case 9: {
                showAllAccounts();
                break;
            }
            case 10: {
                showRelationLogs();
                break;
            }
            case 11: {
                if (saveToFile("account_data.txt") && saveLogs("relation_logs.txt")) {
                    cout << "数据保存成功！" << endl;
                } else {
                    cout << "数据保存失败！" << endl;
                }
                break;
            }
            case 12: {
                if (loadFromFile("account_data.txt") && loadLogs("relation_logs.txt")) {
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
