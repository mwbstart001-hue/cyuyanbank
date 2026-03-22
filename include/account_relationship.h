#ifndef ACCOUNT_RELATIONSHIP_H
#define ACCOUNT_RELATIONSHIP_H

#include "common.h"

struct Account {
    string accountId;
    string customerName;
    string idCard;
    double balance;
    time_t openTime;
    AccountStatus status;
    string remark;

    Account() : balance(0.0), openTime(0), status(ACTIVE) {}
};

struct RelationshipLog {
    string accountId1;
    string accountId2;
    string operation; // BIND, UNBIND
    time_t operationTime;
    string operatorId;

    RelationshipLog() : operationTime(0) {}
};

class UnionFind {
private:
    map<string, string> parent;
    map<string, int> rank;

public:
    string find(const string& x);
    void unite(const string& x, const string& y);
    bool isConnected(const string& x, const string& y);
    void remove(const string& x);
    vector<string> getGroup(const string& x);
    map<string, vector<string>> getAllGroups();
};

class AccountRelationshipModule {
private:
    map<string, Account> accounts;
    UnionFind uf;
    vector<RelationshipLog> relationshipLogs;
    map<string, string> groupRemarks;

    bool hasTransactions(const string& accountId) const;

public:
    AccountRelationshipModule();

    // Account management
    bool createAccount(const string& accountId, const string& customerName,
                       const string& idCard, double initialBalance = 0.0);
    bool deleteAccount(const string& accountId);
    Account* getAccount(const string& accountId);
    const Account* getAccount(const string& accountId) const;
    bool updateAccountRemark(const string& accountId, const string& remark);
    vector<Account> getAllAccounts() const;

    // Relationship management
    bool bindRelationship(const string& accountId1, const string& accountId2,
                          const string& operatorId = "SYSTEM");
    bool unbindRelationship(const string& accountId1, const string& accountId2,
                            const string& operatorId = "SYSTEM");
    vector<string> getRelatedAccounts(const string& accountId) const;
    bool updateGroupRemark(const string& accountId, const string& remark);
    string getGroupRemark(const string& accountId) const;

    // Group operations
    vector<vector<Account>> getAccountGroups() const;
    int getGroupCount() const;

    // Sorting
    vector<Account> getSortedByBalance(bool ascending = false) const;
    vector<Account> getSortedByOpenTime(bool ascending = true) const;

    // Logs
    vector<RelationshipLog> getRelationshipLogs() const;

    // Statistics
    int getTotalAccounts() const;
    int getActiveAccounts() const;
    double getTotalBalance() const;
};

#endif // ACCOUNT_RELATIONSHIP_H