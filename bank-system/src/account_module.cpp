#include "account_module.h"

string Account::toString() const {
    ostringstream oss;
    oss << accountId << "|"
        << accountName << "|"
        << idCard << "|"
        << balance << "|"
        << createTime.toString() << "|"
        << (isActive ? "1" : "0") << "|"
        << remark;
    return oss.str();
}

Account Account::fromString(const string& str) {
    Account acc;
    vector<string> parts = splitString(str, '|');
    if (parts.size() >= 7) {
        acc.accountId = parts[0];
        acc.accountName = parts[1];
        acc.idCard = parts[2];
        acc.balance = stod(parts[3]);
        acc.createTime = Date::fromString(parts[4]);
        acc.isActive = (parts[5] == "1");
        acc.remark = parts[6];
    }
    return acc;
}

string RelationLog::toString() const {
    ostringstream oss;
    oss << logId << "|"
        << account1 << "|"
        << account2 << "|"
        << operation << "|"
        << operateTime.toString() << "|"
        << operator_name;
    return oss.str();
}

RelationLog RelationLog::fromString(const string& str) {
    RelationLog log;
    vector<string> parts = splitString(str, '|');
    if (parts.size() >= 6) {
        log.logId = parts[0];
        log.account1 = parts[1];
        log.account2 = parts[2];
        log.operation = parts[3];
        log.operateTime = Date::fromString(parts[4]);
        log.operator_name = parts[5];
    }
    return log;
}

string UnionFind::find(const string& x) {
    if (parent.find(x) == parent.end()) {
        parent[x] = x;
        rank[x] = 0;
        return x;
    }
    
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void UnionFind::unionSets(const string& x, const string& y) {
    string rootX = find(x);
    string rootY = find(y);
    
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

void UnionFind::addAccount(const string& accountId) {
    if (parent.find(accountId) == parent.end()) {
        parent[accountId] = accountId;
        rank[accountId] = 0;
    }
}

void UnionFind::removeAccount(const string& accountId) {
    parent.erase(accountId);
    rank.erase(accountId);
    groupRemark.erase(accountId);
}

bool UnionFind::areRelated(const string& a, const string& b) {
    if (parent.find(a) == parent.end() || parent.find(b) == parent.end()) {
        return false;
    }
    return find(a) == find(b);
}

vector<string> UnionFind::getRelatedAccounts(const string& accountId) {
    vector<string> result;
    if (parent.find(accountId) == parent.end()) {
        return result;
    }
    
    string root = find(accountId);
    for (const auto& pair : parent) {
        if (find(pair.first) == root && pair.first != accountId) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void UnionFind::setGroupRemark(const string& accountId, const string& remark) {
    string root = find(accountId);
    groupRemark[root] = remark;
}

string UnionFind::getGroupRemark(const string& accountId) {
    string root = find(accountId);
    if (groupRemark.find(root) != groupRemark.end()) {
        return groupRemark[root];
    }
    return "";
}

void UnionFind::clear() {
    parent.clear();
    rank.clear();
    groupRemark.clear();
}

AccountManager::AccountManager() 
    : accountFile("data/accounts.txt"),
      relationFile("data/relations.txt") {
    loadAccounts();
    loadRelations();
}

AccountManager::~AccountManager() {
    saveAccounts();
    saveRelations();
}

bool AccountManager::hasTransactions(const string& accountId) {
    return false;
}

void AccountManager::logRelationChange(const string& a1, const string& a2, const string& op) {
    RelationLog log;
    log.logId = generateId("RL");
    log.account1 = a1;
    log.account2 = a2;
    log.operation = op;
    log.operateTime = Date::now();
    log.operator_name = "system";
    relationLogs.push_back(log);
    saveRelations();
}

string AccountManager::createAccount(const string& name, const string& idCard, double initialBalance) {
    string accountId = generateId("ACC");
    
    Account acc;
    acc.accountId = accountId;
    acc.accountName = name;
    acc.idCard = idCard;
    acc.balance = initialBalance;
    acc.createTime = Date::now();
    acc.isActive = true;
    
    accounts[accountId] = acc;
    unionFind.addAccount(accountId);
    saveAccounts();
    
    return accountId;
}

bool AccountManager::closeAccount(const string& accountId) {
    if (accounts.find(accountId) == accounts.end()) {
        return false;
    }
    
    if (!accounts[accountId].isActive) {
        return false;
    }
    
    if (accounts[accountId].balance != 0) {
        cout << "账户余额不为零，无法注销！" << endl;
        return false;
    }
    
    vector<string> related = unionFind.getRelatedAccounts(accountId);
    for (const auto& rel : related) {
        logRelationChange(accountId, rel, "unbind_on_close");
    }
    
    accounts[accountId].isActive = false;
    unionFind.removeAccount(accountId);
    saveAccounts();
    
    return true;
}

bool AccountManager::bindRelation(const string& a1, const string& a2) {
    if (accounts.find(a1) == accounts.end() || accounts.find(a2) == accounts.end()) {
        return false;
    }
    
    if (!accounts[a1].isActive || !accounts[a2].isActive) {
        return false;
    }
    
    if (unionFind.areRelated(a1, a2)) {
        cout << "账户已存在关联关系！" << endl;
        return false;
    }
    
    unionFind.unionSets(a1, a2);
    logRelationChange(a1, a2, "bind");
    saveAccounts();
    
    return true;
}

bool AccountManager::unbindRelation(const string& a1, const string& a2) {
    if (!unionFind.areRelated(a1, a2)) {
        return false;
    }
    
    string remark = unionFind.getGroupRemark(a1);
    unionFind.removeAccount(a1);
    unionFind.removeAccount(a2);
    
    unionFind.addAccount(a1);
    unionFind.addAccount(a2);
    
    logRelationChange(a1, a2, "unbind");
    saveAccounts();
    
    return true;
}

Account* AccountManager::getAccount(const string& accountId) {
    if (accounts.find(accountId) != accounts.end()) {
        return &accounts[accountId];
    }
    return nullptr;
}

vector<Account> AccountManager::getAllAccounts(SortOrder order, const string& sortBy) {
    vector<Account> result;
    for (const auto& pair : accounts) {
        if (pair.second.isActive) {
            result.push_back(pair.second);
        }
    }
    
    if (sortBy == "balance") {
        sort(result.begin(), result.end(), 
             [order](const Account& a, const Account& b) -> bool {
                 if (order == SortOrder::ASC) {
                     return a.balance < b.balance;
                 }
                 return a.balance > b.balance;
             });
    } else {
        sort(result.begin(), result.end(), 
             [order](const Account& a, const Account& b) -> bool {
                 if (order == SortOrder::ASC) {
                     return a.createTime < b.createTime;
                 }
                 return a.createTime > b.createTime;
             });
    }
    
    return result;
}

vector<Account> AccountManager::getRelatedAccountList(const string& accountId) {
    vector<Account> result;
    vector<string> related = unionFind.getRelatedAccounts(accountId);
    
    for (const auto& id : related) {
        if (accounts.find(id) != accounts.end() && accounts[id].isActive) {
            result.push_back(accounts[id]);
        }
    }
    
    return result;
}

bool AccountManager::updateBalance(const string& accountId, double amount) {
    if (accounts.find(accountId) == accounts.end() || !accounts[accountId].isActive) {
        return false;
    }
    
    accounts[accountId].balance += amount;
    if (accounts[accountId].balance < 0) {
        accounts[accountId].balance -= amount;
        return false;
    }
    
    saveAccounts();
    return true;
}

bool AccountManager::updateRemark(const string& accountId, const string& remark) {
    if (accounts.find(accountId) == accounts.end()) {
        return false;
    }
    
    accounts[accountId].remark = remark;
    saveAccounts();
    return true;
}

bool AccountManager::updateGroupRemark(const string& accountId, const string& groupRemark) {
    if (accounts.find(accountId) == accounts.end()) {
        return false;
    }
    
    unionFind.setGroupRemark(accountId, groupRemark);
    saveAccounts();
    return true;
}

bool AccountManager::loadAccounts() {
    ifstream file(accountFile);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        Account acc = Account::fromString(line);
        accounts[acc.accountId] = acc;
        if (acc.isActive) {
            unionFind.addAccount(acc.accountId);
        }
    }
    
    file.close();
    return true;
}

bool AccountManager::saveAccounts() {
    ofstream file(accountFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : accounts) {
        file << pair.second.toString() << "\n";
    }
    
    file.close();
    return true;
}

bool AccountManager::loadRelations() {
    ifstream file(relationFile);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        RelationLog log = RelationLog::fromString(line);
        relationLogs.push_back(log);
    }
    
    file.close();
    return true;
}

bool AccountManager::saveRelations() {
    ofstream file(relationFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& log : relationLogs) {
        file << log.toString() << "\n";
    }
    
    file.close();
    return true;
}

void AccountManager::displayAccount(const string& accountId) {
    Account* acc = getAccount(accountId);
    if (!acc) {
        cout << "账户不存在！" << endl;
        return;
    }
    
    cout << "\n========== 账户详情 ==========" << endl;
    cout << "账户ID: " << acc->accountId << endl;
    cout << "账户名: " << acc->accountName << endl;
    cout << "身份证: " << acc->idCard << endl;
    cout << "余额: " << fixed << setprecision(2) << acc->balance << endl;
    cout << "开户时间: " << acc->createTime.toString() << endl;
    cout << "状态: " << (acc->isActive ? "正常" : "已注销") << endl;
    cout << "备注: " << acc->remark << endl;
    
    string groupRemark = unionFind.getGroupRemark(accountId);
    if (!groupRemark.empty()) {
        cout << "关联组备注: " << groupRemark << endl;
    }
    
    vector<Account> related = getRelatedAccountList(accountId);
    if (!related.empty()) {
        cout << "\n关联账户:" << endl;
        for (const auto& rel : related) {
            cout << "  - " << rel.accountId << " (" << rel.accountName << ")" << endl;
        }
    }
}

void AccountManager::displayAllAccounts() {
    cout << "\n========== 所有账户列表 ==========" << endl;
    vector<Account> accs = getAllAccounts();
    
    if (accs.empty()) {
        cout << "暂无账户" << endl;
        return;
    }
    
    cout << left << setw(12) << "账户ID" 
         << setw(15) << "账户名"
         << setw(20) << "身份证"
         << setw(15) << "余额"
         << setw(20) << "开户时间" << endl;
    cout << string(82, '-') << endl;
    
    for (const auto& acc : accs) {
        cout << left << setw(12) << acc.accountId
             << setw(15) << acc.accountName
             << setw(20) << acc.idCard
             << setw(15) << fixed << setprecision(2) << acc.balance
             << setw(20) << acc.createTime.toString() << endl;
    }
}

void AccountManager::displayRelations(const string& accountId) {
    cout << "\n========== 关联关系 ==========" << endl;
    
    vector<Account> related = getRelatedAccountList(accountId);
    if (related.empty()) {
        cout << "无关联账户" << endl;
        return;
    }
    
    string groupRemark = unionFind.getGroupRemark(accountId);
    if (!groupRemark.empty()) {
        cout << "关联组备注: " << groupRemark << endl;
    }
    
    cout << "\n关联账户列表:" << endl;
    for (const auto& acc : related) {
        cout << "  " << acc.accountId << " - " << acc.accountName 
             << " (余额: " << fixed << setprecision(2) << acc.balance << ")" << endl;
    }
}

void AccountManager::displayRelationLogs() {
    cout << "\n========== 关联变更记录 ==========" << endl;
    
    if (relationLogs.empty()) {
        cout << "暂无变更记录" << endl;
        return;
    }
    
    cout << left << setw(12) << "记录ID"
         << setw(12) << "账户1"
         << setw(12) << "账户2"
         << setw(10) << "操作"
         << setw(20) << "时间" << endl;
    cout << string(66, '-') << endl;
    
    for (const auto& log : relationLogs) {
        cout << left << setw(12) << log.logId
             << setw(12) << log.account1
             << setw(12) << log.account2
             << setw(10) << log.operation
             << setw(20) << log.operateTime.toString() << endl;
    }
}
