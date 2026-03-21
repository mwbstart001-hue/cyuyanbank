#ifndef BANK_MODELS_H
#define BANK_MODELS_H

#include <string>

// 账户状态枚举
enum class AccountStatus : int {
    CLOSED = 0, // 已注销
    ACTIVE = 1  // 正常
};

// 账户模型
struct Account {
    int id;
    std::string name;
    double balance;
    int creditScore;
    AccountStatus status;

    Account(int id, std::string name, double balance, int creditScore, int statusInt = 1)
        : id(id), name(name), balance(balance), creditScore(creditScore), 
          status(static_cast<AccountStatus>(statusInt)) {}
};

#endif // BANK_MODELS_H
