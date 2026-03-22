#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "common.h"

// 账户结构体
struct Account {
    int id;
    string name;
    double balance;
    long openTime;
    string remark;
    bool active;
    int transactionCount;
    
    Account() : id(0), balance(0), openTime(0), active(true), transactionCount(0) {}
    
    Account(int i, const string& n, double b) : id(i), name(n), balance(b), 
                                                 openTime(Utils::getCurrentTimestamp()),
                                                 active(true), transactionCount(0) {}
};

// 关联关系变更记录
struct RelationLog {
    int accountA;
    int accountB;
    string operation;
    long timestamp;
    string remark;
    
    RelationLog() : accountA(0), accountB(0), timestamp(0) {}
    
    RelationLog(int a, int b, const string& op, const string& r = "") 
        : accountA(a), accountB(b), operation(op), 
          timestamp(Utils::getCurrentTimestamp()), remark(r) {}
};

// 并查集实现的账户关系管理
class UnionFindSystem : public FileStorage {
private:
    Account* accounts;
    int* parent;
    int* rank;
    bool* active;
    int capacity;
    int count;
    
    RelationLog* logs;
    int logCount;
    int logCapacity;
    
    int find(int x);
    void unionSets(int x, int y);
    
public:
    UnionFindSystem(int cap = 1000);
    ~UnionFindSystem();
    
    // 账户管理
    bool addAccount(int id, const string& name, double balance);
    bool removeAccount(int id);
    bool updateAccount(int id, const string& field, const string& value);
    Account* getAccount(int id);
    
    // 关联关系
    bool bindRelation(int id1, int id2, const string& remark = "");
    bool unbindRelation(int id1, int id2, const string& remark = "");
    bool isRelated(int id1, int id2);
    void queryRelatedAccounts(int id);
    void updateGroupRemark(int id, const string& remark);
    
    // 查询与排序
    void queryByBalance();
    void queryByOpenTime();
    void showAllAccounts();
    void showRelationLogs();
    
    // 校验
    bool canDeleteAccount(int id);
    
    // 文件存储
    bool saveToFile(const string& filename) override;
    bool loadFromFile(const string& filename) override;
    bool saveLogs(const string& filename);
    bool loadLogs(const string& filename);
    
    // 菜单接口
    void showMenu();
    void run();
};

#endif
