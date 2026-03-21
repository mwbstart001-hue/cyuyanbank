#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "Account.h"

/*
 * 数据库管理类（Database Manager）
 * 
 * 【开发者A 负责】
 * 
 * 用途：封装 SQLite 数据库操作，提供银行账户的增删改查功能。
 * 
 * 改进说明：
 *   - 所有操作前增加"账户存在性"校验，防止操作不存在的账户
 *   - deleteAccount 返回 bool，防止重复销户
 *   - 新增 accountExists() 和 getAccount() 方法
 *   - 新增 transfer() 原子转账方法（事务保护）
 *   - 使用参数化查询防止 SQL 注入
 */
class DatabaseManager {
private:
    sqlite3* db;

    // 执行简单 SQL（无返回值的）
    void execSQL(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string err = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL 执行失败: " + err);
        }
    }

public:
    DatabaseManager(const std::string& dbPath) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            throw std::runtime_error("无法打开数据库: " + std::string(sqlite3_errmsg(db)));
        }
        createTable();
    }

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    // 创建账户表（如果不存在）
    void createTable() {
        const char* sql = "CREATE TABLE IF NOT EXISTS accounts ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "name TEXT NOT NULL,"
                          "balance REAL DEFAULT 0,"
                          "credit_score INTEGER DEFAULT 600,"
                          "status INTEGER DEFAULT 1);";
        execSQL(sql);

        // 为了兼容旧数据，尝试添加 status 列，如果已存在会静默失败
        try {
            execSQL("ALTER TABLE accounts ADD COLUMN status INTEGER DEFAULT 1;");
        } catch (...) {
            // 列可能已经存在，忽略此错误
        }
    }

    // ===================== 查询类方法 =====================

    // 检查账户是否存在
    bool accountExists(int id) {
        const char* sql = "SELECT COUNT(*) FROM accounts WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("查询准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_int(stmt, 1, id);

        bool exists = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            exists = (sqlite3_column_int(stmt, 0) > 0);
        }
        sqlite3_finalize(stmt);
        return exists;
    }

    // 获取单个账户信息
    Account getAccount(int id) {
        if (!accountExists(id)) {
            throw std::runtime_error("账户 ID " + std::to_string(id) + " 不存在!");
        }

        const char* sql = "SELECT id, name, balance, credit_score, status FROM accounts WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("查询准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_int(stmt, 1, id);

        Account acc(0, "", 0, 0, 0);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            acc.id = sqlite3_column_int(stmt, 0);
            acc.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            acc.balance = sqlite3_column_double(stmt, 2);
            acc.creditScore = sqlite3_column_int(stmt, 3);
            acc.status = static_cast<AccountStatus>(sqlite3_column_int(stmt, 4));
        }
        sqlite3_finalize(stmt);
        return acc;
    }

    // 获取所有账户
    std::vector<Account> getAllAccounts() {
        std::vector<Account> accounts;
        const char* sql = "SELECT id, name, balance, credit_score, status FROM accounts;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("查询准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double balance = sqlite3_column_double(stmt, 2);
            int score = sqlite3_column_int(stmt, 3);
            int statusInt = sqlite3_column_int(stmt, 4);
            accounts.emplace_back(id, name, balance, score, statusInt);
        }
        sqlite3_finalize(stmt);
        return accounts;
    }

    // ===================== 增加 =====================

    // 开户（返回新账户的 ID）
    int addAccount(const std::string& name, double balance, int creditScore) {
        // 业务校验
        if (name.empty()) {
            throw std::runtime_error("姓名不能为空!");
        }
        if (balance < 0) {
            throw std::runtime_error("初始余额不能为负数!");
        }
        if (creditScore < 0 || creditScore > 1000) {
            throw std::runtime_error("信用分必须在 0-1000 之间!");
        }

        const char* sql = "INSERT INTO accounts (name, balance, credit_score) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("插入准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt, 2, balance);
        sqlite3_bind_int(stmt, 3, creditScore);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("开户失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);

        int newId = static_cast<int>(sqlite3_last_insert_rowid(db));
        return newId;
    }

    // ===================== 删除 =====================

    // 销户（逻辑删除：带存在性及余额检验，防止重复销户）
    void deleteAccount(int id) {
        Account acc = getAccount(id);
        if (acc.status == AccountStatus::CLOSED) {
            throw std::runtime_error("销户失败: 账户 ID " + std::to_string(id) + " 已经处于注销状态!");
        }
        if (acc.balance > 0.001) { // 使用微小偏差防止浮点数精度问题
            throw std::runtime_error("销户失败: 账户 ID " + std::to_string(id) + " 尚有余额 " + std::to_string(acc.balance) + " 元，请取款清空后再注销！");
        }

        const char* sql = "UPDATE accounts SET status = 0 WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("删除准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("销户执行失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);
    }

    // ===================== 修改 =====================

    // 更新余额（带存在性检查）
    void updateBalance(int id, double newBalance) {
        if (!accountExists(id)) {
            throw std::runtime_error("更新失败: 账户 ID " + std::to_string(id) + " 不存在!");
        }

        const char* sql = "UPDATE accounts SET balance = ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("更新准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_double(stmt, 1, newBalance);
        sqlite3_bind_int(stmt, 2, id);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("余额更新失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);
    }

    // 修改信用分（带存在性和范围检查）
    void updateCreditScore(int id, int newScore) {
        if (!accountExists(id)) {
            throw std::runtime_error("更新失败: 账户 ID " + std::to_string(id) + " 不存在!");
        }
        if (newScore < 0 || newScore > 1000) {
            throw std::runtime_error("信用分必须在 0-1000 之间!");
        }

        const char* sql = "UPDATE accounts SET credit_score = ? WHERE id = ?;";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("更新准备失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_bind_int(stmt, 1, newScore);
        sqlite3_bind_int(stmt, 2, id);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("信用分更新失败: " + std::string(sqlite3_errmsg(db)));
        }
        sqlite3_finalize(stmt);
    }

    // ===================== 转账（原子事务） =====================

    // 转账：使用 SQLite 事务保证原子性
    // 转出方扣款与转入方到账要么同时成功，要么同时失败
    void transfer(int fromId, int toId, double amount) {
        // 前置校验
        if (fromId == toId) {
            throw std::runtime_error("转账失败: 不能给自己转账!");
        }
        if (amount <= 0) {
            throw std::runtime_error("转账失败: 转账金额必须大于0!");
        }

        Account fromAcc = getAccount(fromId);  // 内部已含存在性检查
        Account toAcc = getAccount(toId);       // 内部已含存在性检查

        if (fromAcc.status == AccountStatus::CLOSED) {
            throw std::runtime_error("转账失败: 转出账户已被注销，禁止转出!");
        }
        if (toAcc.status == AccountStatus::CLOSED) {
            throw std::runtime_error("转账失败: 接收账户已被注销，禁止转入!");
        }

        if (fromAcc.balance < amount) {
            throw std::runtime_error("转账失败: 账户 " + std::to_string(fromId) 
                + " 余额不足! 当前余额: " + std::to_string(fromAcc.balance) 
                + ", 转账金额: " + std::to_string(amount));
        }

        // 开启事务
        execSQL("BEGIN TRANSACTION;");
        try {
            updateBalance(fromId, fromAcc.balance - amount);
            updateBalance(toId, toAcc.balance + amount);
            execSQL("COMMIT;");
        } catch (...) {
            execSQL("ROLLBACK;");
            throw std::runtime_error("转账执行异常，已回滚!");
        }
    }
};

#endif // DATABASE_MANAGER_H
