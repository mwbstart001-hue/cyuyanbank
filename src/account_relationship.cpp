#include "account_relationship.h"

// UnionFind implementation
string UnionFind::find(const string& x) {
    if (parent.find(x) == parent.end()) {
        parent[x] = x;
        rank[x] = 1;
    }
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void UnionFind::unite(const string& x, const string& y) {
    string xRoot = find(x);
    string yRoot = find(y);

    if (xRoot == yRoot) return;

    if (rank[xRoot] < rank[yRoot]) {
        parent[xRoot] = yRoot;
    } else if (rank[xRoot] > rank[yRoot]) {
        parent[yRoot] = xRoot;
    } else {
        parent[yRoot] = xRoot;
        rank[xRoot]++;
    }
}

bool UnionFind::isConnected(const string& x, const string& y) {
    return find(x) == find(y);
}

void UnionFind::remove(const string& x) {
    string root = find(x);
    vector<string> group = getGroup(x);

    for (const string& member : group) {
        parent.erase(member);
        rank.erase(member);
    }

    for (size_t i = 1; i < group.size(); i++) {
        if (group[i] != x) {
            find(group[i]);
        }
    }

    for (size_t i = 1; i < group.size(); i++) {
        for (size_t j = i + 1; j < group.size(); j++) {
            if (group[i] != x && group[j] != x) {
                unite(group[i], group[j]);
            }
        }
    }
}

vector<string> UnionFind::getGroup(const string& x) {
    vector<string> group;
    string root = find(x);

    for (const auto& pair : parent) {
        if (find(pair.first) == root) {
            group.push_back(pair.first);
        }
    }
    return group;
}

map<string, vector<string>> UnionFind::getAllGroups() {
    map<string, vector<string>> groups;

    for (const auto& pair : parent) {
        string root = find(pair.first);
        groups[root].push_back(pair.first);
    }
    return groups;
}

// AccountRelationshipModule implementation
AccountRelationshipModule::AccountRelationshipModule() {}

bool AccountRelationshipModule::hasTransactions(const string& accountId) const {
    return false;
}

bool AccountRelationshipModule::createAccount(const string& accountId, const string& customerName,
                                              const string& idCard, double initialBalance) {
    if (accounts.find(accountId) != accounts.end()) {
        return false;
    }

    Account acc;
    acc.accountId = accountId;
    acc.customerName = customerName;
    acc.idCard = idCard;
    acc.balance = initialBalance;
    acc.openTime = time(nullptr);
    acc.status = ACTIVE;

    accounts[accountId] = acc;
    uf.find(accountId);
    return true;
}

bool AccountRelationshipModule::deleteAccount(const string& accountId) {
    auto it = accounts.find(accountId);
    if (it == accounts.end()) {
        return false;
    }

    if (hasTransactions(accountId)) {
        return false;
    }

    it->second.status = CLOSED;
    uf.remove(accountId);
    return true;
}

Account* AccountRelationshipModule::getAccount(const string& accountId) {
    auto it = accounts.find(accountId);
    if (it != accounts.end()) {
        return &it->second;
    }
    return nullptr;
}

const Account* AccountRelationshipModule::getAccount(const string& accountId) const {
    auto it = accounts.find(accountId);
    if (it != accounts.end()) {
        return &it->second;
    }
    return nullptr;
}

bool AccountRelationshipModule::updateAccountRemark(const string& accountId, const string& remark) {
    Account* acc = getAccount(accountId);
    if (acc) {
        acc->remark = remark;
        return true;
    }
    return false;
}

vector<Account> AccountRelationshipModule::getAllAccounts() const {
    vector<Account> result;
    for (const auto& pair : accounts) {
        result.push_back(pair.second);
    }
    return result;
}

bool AccountRelationshipModule::bindRelationship(const string& accountId1, const string& accountId2,
                                                  const string& operatorId) {
    if (accounts.find(accountId1) == accounts.end() ||
        accounts.find(accountId2) == accounts.end()) {
        return false;
    }

    if (uf.isConnected(accountId1, accountId2)) {
        return false;
    }

    uf.unite(accountId1, accountId2);

    RelationshipLog log;
    log.accountId1 = accountId1;
    log.accountId2 = accountId2;
    log.operation = "BIND";
    log.operationTime = time(nullptr);
    log.operatorId = operatorId;
    relationshipLogs.push_back(log);

    return true;
}

bool AccountRelationshipModule::unbindRelationship(const string& accountId1, const string& accountId2,
                                                    const string& operatorId) {
    if (accounts.find(accountId1) == accounts.end() ||
        accounts.find(accountId2) == accounts.end()) {
        return false;
    }

    if (!uf.isConnected(accountId1, accountId2)) {
        return false;
    }

    vector<string> group = uf.getGroup(accountId1);
    uf.remove(accountId1);
    uf.remove(accountId2);

    for (size_t i = 0; i < group.size(); i++) {
        for (size_t j = i + 1; j < group.size(); j++) {
            if ((group[i] != accountId1 || group[j] != accountId2) &&
                (group[i] != accountId2 || group[j] != accountId1)) {
                uf.unite(group[i], group[j]);
            }
        }
    }

    RelationshipLog log;
    log.accountId1 = accountId1;
    log.accountId2 = accountId2;
    log.operation = "UNBIND";
    log.operationTime = time(nullptr);
    log.operatorId = operatorId;
    relationshipLogs.push_back(log);

    return true;
}

vector<string> AccountRelationshipModule::getRelatedAccounts(const string& accountId) const {
    if (accounts.find(accountId) == accounts.end()) {
        return {};
    }

    vector<string> result;
    vector<string> group = const_cast<UnionFind&>(uf).getGroup(accountId);
    for (const string& id : group) {
        if (id != accountId) {
            result.push_back(id);
        }
    }
    return result;
}

bool AccountRelationshipModule::updateGroupRemark(const string& accountId, const string& remark) {
    if (accounts.find(accountId) == accounts.end()) {
        return false;
    }

    string root = const_cast<UnionFind&>(uf).find(accountId);
    groupRemarks[root] = remark;
    return true;
}

string AccountRelationshipModule::getGroupRemark(const string& accountId) const {
    if (accounts.find(accountId) == accounts.end()) {
        return "";
    }

    string root = const_cast<UnionFind&>(uf).find(accountId);
    auto it = groupRemarks.find(root);
    if (it != groupRemarks.end()) {
        return it->second;
    }
    return "";
}

vector<vector<Account>> AccountRelationshipModule::getAccountGroups() const {
    vector<vector<Account>> result;
    auto groups = const_cast<UnionFind&>(uf).getAllGroups();

    for (const auto& pair : groups) {
        vector<Account> group;
        for (const string& accountId : pair.second) {
            auto it = accounts.find(accountId);
            if (it != accounts.end()) {
                group.push_back(it->second);
            }
        }
        if (!group.empty()) {
            result.push_back(group);
        }
    }

    for (const auto& pair : accounts) {
        bool found = false;
        for (const auto& group : result) {
            for (const auto& acc : group) {
                if (acc.accountId == pair.first) {
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        if (!found && pair.second.status != CLOSED) {
            result.push_back({pair.second});
        }
    }

    return result;
}

int AccountRelationshipModule::getGroupCount() const {
    return getAccountGroups().size();
}

vector<Account> AccountRelationshipModule::getSortedByBalance(bool ascending) const {
    vector<Account> result = getAllAccounts();
    sort(result.begin(), result.end(), [ascending](const Account& a, const Account& b) {
        return ascending ? (a.balance < b.balance) : (a.balance > b.balance);
    });
    return result;
}

vector<Account> AccountRelationshipModule::getSortedByOpenTime(bool ascending) const {
    vector<Account> result = getAllAccounts();
    sort(result.begin(), result.end(), [ascending](const Account& a, const Account& b) {
        return ascending ? (a.openTime < b.openTime) : (a.openTime > b.openTime);
    });
    return result;
}

vector<RelationshipLog> AccountRelationshipModule::getRelationshipLogs() const {
    return relationshipLogs;
}

int AccountRelationshipModule::getTotalAccounts() const {
    return accounts.size();
}

int AccountRelationshipModule::getActiveAccounts() const {
    int count = 0;
    for (const auto& pair : accounts) {
        if (pair.second.status == ACTIVE) {
            count++;
        }
    }
    return count;
}

double AccountRelationshipModule::getTotalBalance() const {
    double total = 0.0;
    for (const auto& pair : accounts) {
        if (pair.second.status == ACTIVE) {
            total += pair.second.balance;
        }
    }
    return total;
}