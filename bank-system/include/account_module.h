#ifndef ACCOUNT_MODULE_H
#define ACCOUNT_MODULE_H

#include "common.h"

struct Account {
    string accountId;
    string accountName;
    string idCard;
    double balance;
    Date createTime;
    bool isActive;
    string remark;
    
    Account() : balance(0.0), isActive(true) {}
    
    string toString() const;
    static Account fromString(const string& str);
    
    bool operator<(const Account& other) const {
        return createTime < other.createTime;
    }
};

struct RelationLog {
    string logId;
    string account1;
    string account2;
    string operation;
    Date operateTime;
    string operator_name;
    
    string toString() const;
    static RelationLog fromString(const string& str);
};

class UnionFind {
private:
    map<string, string> parent;
    map<string, int> rank;
    map<string, string> groupRemark;
    
    string find(const string& x);
    
public:
    void addAccount(const string& accountId);
    void removeAccount(const string& accountId);
    bool areRelated(const string& a, const string& b);
    vector<string> getRelatedAccounts(const string& accountId);
    void setGroupRemark(const string& accountId, const string& remark);
    string getGroupRemark(const string& accountId);
    void unionSets(const string& x, const string& y);
    void clear();
};

class AccountManager {
private:
    map<string, Account> accounts;
    vector<RelationLog> relationLogs;
    UnionFind unionFind;
    
    string accountFile;
    string relationFile;
    
    bool hasTransactions(const string& accountId);
    void logRelationChange(const string& a1, const string& a2, const string& op);
    
public:
    AccountManager();
    ~AccountManager();
    
    string createAccount(const string& name, const string& idCard, double initialBalance = 0);
    bool closeAccount(const string& accountId);
    bool bindRelation(const string& a1, const string& a2);
    bool unbindRelation(const string& a1, const string& a2);
    
    Account* getAccount(const string& accountId);
    vector<Account> getAllAccounts(SortOrder order = SortOrder::ASC, 
                                    const string& sortBy = "time");
    vector<Account> getRelatedAccountList(const string& accountId);
    
    bool updateBalance(const string& accountId, double amount);
    bool updateRemark(const string& accountId, const string& remark);
    bool updateGroupRemark(const string& accountId, const string& groupRemark);
    
    bool loadAccounts();
    bool saveAccounts();
    bool loadRelations();
    bool saveRelations();
    
    void displayAccount(const string& accountId);
    void displayAllAccounts();
    void displayRelations(const string& accountId);
    void displayRelationLogs();
};

#endif
