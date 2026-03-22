#include "common.h"
#include "queue_module.h"
#include "account_module.h"
#include "transaction_module.h"
#include "risk_module.h"

class BankSystem {
private:
    CircularQueue queue;
    AccountManager accountManager;
    TransactionManager transactionManager;
    RiskAssessment riskAssessment;
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void pause() {
        cout << "\n按回车键继续...";
        cin.ignore();
        cin.get();
    }
    
    void showMainMenu() {
        cout << "\n╔══════════════════════════════════════╗" << endl;
        cout << "║       银行综合管理系统 v1.0          ║" << endl;
        cout << "╠══════════════════════════════════════╣" << endl;
        cout << "║  1. 排队叫号管理                     ║" << endl;
        cout << "║  2. 账户关系管理                     ║" << endl;
        cout << "║  3. 交易流水管理                     ║" << endl;
        cout << "║  4. 风险评估管理                     ║" << endl;
        cout << "║  0. 退出系统                         ║" << endl;
        cout << "╚══════════════════════════════════════╝" << endl;
        cout << "请选择: ";
    }
    
    void queueMenu() {
        while (true) {
            cout << "\n========== 排队叫号管理 ==========" << endl;
            cout << "1. 取号（普通）" << endl;
            cout << "2. 取号（VIP）" << endl;
            cout << "3. 叫号" << endl;
            cout << "4. 查询排队位置" << endl;
            cout << "5. 取消排队" << endl;
            cout << "6. 普通转VIP" << endl;
            cout << "7. 查看排队信息" << endl;
            cout << "8. 查看统计信息" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    string name;
                    cout << "请输入客户姓名: ";
                    cin >> name;
                    string ticket = queue.takeTicket(name, CustomerType::NORMAL);
                    if (!ticket.empty()) {
                        cout << "取号成功！票号: " << ticket << endl;
                    } else {
                        cout << "取号失败，队列已满！" << endl;
                    }
                    break;
                }
                case 2: {
                    string name;
                    cout << "请输入客户姓名: ";
                    cin >> name;
                    string ticket = queue.takeTicket(name, CustomerType::VIP);
                    if (!ticket.empty()) {
                        cout << "取号成功！票号: " << ticket << endl;
                    } else {
                        cout << "取号失败，队列已满！" << endl;
                    }
                    break;
                }
                case 3: {
                    int window;
                    cout << "请输入窗口号 (1-5): ";
                    cin >> window;
                    if (!queue.callNext(window - 1)) {
                        cout << "叫号失败，无等待客户或窗口号无效！" << endl;
                    }
                    break;
                }
                case 4: {
                    string ticketId;
                    cout << "请输入票号: ";
                    cin >> ticketId;
                    int pos = queue.queryPosition(ticketId);
                    if (pos > 0) {
                        cout << "当前排队位置: " << pos << endl;
                    } else {
                        QueueNode* node = queue.queryTicket(ticketId);
                        if (node) {
                            cout << "该票号已不在等待队列中" << endl;
                        } else {
                            cout << "票号不存在！" << endl;
                        }
                    }
                    break;
                }
                case 5: {
                    string ticketId;
                    cout << "请输入票号: ";
                    cin >> ticketId;
                    if (queue.cancelTicket(ticketId)) {
                        cout << "取消成功！" << endl;
                    } else {
                        cout << "取消失败！" << endl;
                    }
                    break;
                }
                case 6: {
                    string ticketId;
                    cout << "请输入票号: ";
                    cin >> ticketId;
                    if (queue.upgradeToVIP(ticketId)) {
                        cout << "升级成功！" << endl;
                    } else {
                        cout << "升级失败！" << endl;
                    }
                    break;
                }
                case 7:
                    queue.displayQueue();
                    break;
                case 8:
                    queue.displayStatistics();
                    break;
                case 0:
                    return;
                default:
                    cout << "无效选择！" << endl;
            }
            pause();
        }
    }
    
    void accountMenu() {
        while (true) {
            cout << "\n========== 账户关系管理 ==========" << endl;
            cout << "1. 开设账户" << endl;
            cout << "2. 注销账户" << endl;
            cout << "3. 查询账户" << endl;
            cout << "4. 查看所有账户" << endl;
            cout << "5. 绑定关联关系" << endl;
            cout << "6. 解除关联关系" << endl;
            cout << "7. 查看关联账户" << endl;
            cout << "8. 修改关联组备注" << endl;
            cout << "9. 查看关联变更记录" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    string name, idCard;
                    double balance;
                    cout << "请输入账户名: ";
                    cin >> name;
                    cout << "请输入身份证号: ";
                    cin >> idCard;
                    cout << "请输入初始余额: ";
                    cin >> balance;
                    
                    string accId = accountManager.createAccount(name, idCard, balance);
                    cout << "开户成功！账户ID: " << accId << endl;
                    break;
                }
                case 2: {
                    string accId;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    if (accountManager.closeAccount(accId)) {
                        cout << "注销成功！" << endl;
                    } else {
                        cout << "注销失败！" << endl;
                    }
                    break;
                }
                case 3: {
                    string accId;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    accountManager.displayAccount(accId);
                    break;
                }
                case 4:
                    accountManager.displayAllAccounts();
                    break;
                case 5: {
                    string a1, a2;
                    cout << "请输入第一个账户ID: ";
                    cin >> a1;
                    cout << "请输入第二个账户ID: ";
                    cin >> a2;
                    if (accountManager.bindRelation(a1, a2)) {
                        cout << "绑定成功！" << endl;
                    } else {
                        cout << "绑定失败！" << endl;
                    }
                    break;
                }
                case 6: {
                    string a1, a2;
                    cout << "请输入第一个账户ID: ";
                    cin >> a1;
                    cout << "请输入第二个账户ID: ";
                    cin >> a2;
                    if (accountManager.unbindRelation(a1, a2)) {
                        cout << "解除成功！" << endl;
                    } else {
                        cout << "解除失败！" << endl;
                    }
                    break;
                }
                case 7: {
                    string accId;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    accountManager.displayRelations(accId);
                    break;
                }
                case 8: {
                    string accId, remark;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    cout << "请输入关联组备注: ";
                    cin.ignore();
                    getline(cin, remark);
                    if (accountManager.updateGroupRemark(accId, remark)) {
                        cout << "修改成功！" << endl;
                    } else {
                        cout << "修改失败！" << endl;
                    }
                    break;
                }
                case 9:
                    accountManager.displayRelationLogs();
                    break;
                case 0:
                    return;
                default:
                    cout << "无效选择！" << endl;
            }
            pause();
        }
    }
    
    void transactionMenu() {
        while (true) {
            cout << "\n========== 交易流水管理 ==========" << endl;
            cout << "1. 存款" << endl;
            cout << "2. 取款" << endl;
            cout << "3. 转账" << endl;
            cout << "4. 查询交易记录" << endl;
            cout << "5. 查看账户流水" << endl;
            cout << "6. 查看所有流水" << endl;
            cout << "7. 删除交易记录" << endl;
            cout << "8. 压缩交易数据" << endl;
            cout << "9. 解压交易数据" << endl;
            cout << "10. 查看压缩统计" << endl;
            cout << "11. 查看交易统计" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    string accId;
                    double amount;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    cout << "请输入存款金额: ";
                    cin >> amount;
                    
                    Account* acc = accountManager.getAccount(accId);
                    if (!acc) {
                        cout << "账户不存在！" << endl;
                        break;
                    }
                    
                    if (accountManager.updateBalance(accId, amount)) {
                        acc = accountManager.getAccount(accId);
                        transactionManager.addTransaction(accId, TransactionType::DEPOSIT, 
                                                         amount, acc->balance, "存款");
                        cout << "存款成功！当前余额: " << acc->balance << endl;
                    } else {
                        cout << "存款失败！" << endl;
                    }
                    break;
                }
                case 2: {
                    string accId;
                    double amount;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    cout << "请输入取款金额: ";
                    cin >> amount;
                    
                    Account* acc = accountManager.getAccount(accId);
                    if (!acc) {
                        cout << "账户不存在！" << endl;
                        break;
                    }
                    
                    if (accountManager.updateBalance(accId, -amount)) {
                        acc = accountManager.getAccount(accId);
                        transactionManager.addTransaction(accId, TransactionType::WITHDRAW, 
                                                         amount, acc->balance, "取款");
                        cout << "取款成功！当前余额: " << acc->balance << endl;
                    } else {
                        cout << "取款失败，余额不足！" << endl;
                    }
                    break;
                }
                case 3: {
                    string fromId, toId;
                    double amount;
                    cout << "请输入转出账户ID: ";
                    cin >> fromId;
                    cout << "请输入转入账户ID: ";
                    cin >> toId;
                    cout << "请输入转账金额: ";
                    cin >> amount;
                    
                    Account* fromAcc = accountManager.getAccount(fromId);
                    Account* toAcc = accountManager.getAccount(toId);
                    
                    if (!fromAcc || !toAcc) {
                        cout << "账户不存在！" << endl;
                        break;
                    }
                    
                    if (accountManager.updateBalance(fromId, -amount)) {
                        accountManager.updateBalance(toId, amount);
                        fromAcc = accountManager.getAccount(fromId);
                        toAcc = accountManager.getAccount(toId);
                        
                        transactionManager.addTransaction(fromId, TransactionType::TRANSFER_OUT, 
                                                         amount, fromAcc->balance, "转账", toId);
                        transactionManager.addTransaction(toId, TransactionType::TRANSFER_IN, 
                                                         amount, toAcc->balance, "转账", fromId);
                        cout << "转账成功！" << endl;
                    } else {
                        cout << "转账失败，余额不足！" << endl;
                    }
                    break;
                }
                case 4: {
                    string transId;
                    cout << "请输入交易ID: ";
                    cin >> transId;
                    transactionManager.displayTransaction(transId);
                    break;
                }
                case 5: {
                    string accId;
                    cout << "请输入账户ID: ";
                    cin >> accId;
                    transactionManager.displayAccountTransactions(accId);
                    break;
                }
                case 6:
                    transactionManager.displayAllTransactions();
                    break;
                case 7: {
                    string transId;
                    cout << "请输入交易ID: ";
                    cin >> transId;
                    if (transactionManager.deleteTransaction(transId)) {
                        cout << "删除成功（已标记）！" << endl;
                    } else {
                        cout << "删除失败！" << endl;
                    }
                    break;
                }
                case 8:
                    transactionManager.compressTransactions();
                    break;
                case 9:
                    transactionManager.decompressTransactions();
                    break;
                case 10:
                    transactionManager.showCompressionStats();
                    break;
                case 11:
                    transactionManager.displayStatistics();
                    break;
                case 0:
                    return;
                default:
                    cout << "无效选择！" << endl;
            }
            pause();
        }
    }
    
    void riskMenu() {
        while (true) {
            cout << "\n========== 风险评估管理 ==========" << endl;
            cout << "1. 添加客户" << endl;
            cout << "2. 更新客户信息" << endl;
            cout << "3. 删除客户" << endl;
            cout << "4. 查询客户" << endl;
            cout << "5. 查看所有客户" << endl;
            cout << "6. 评估客户风险" << endl;
            cout << "7. 批量评估" << endl;
            cout << "8. 查看高风险客户" << endl;
            cout << "9. 按信用等级查询" << endl;
            cout << "10. 调整评估规则" << endl;
            cout << "11. 查看评估规则" << endl;
            cout << "12. 查看统计信息" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "请选择: ";
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1: {
                    string name, idCard;
                    double income, assets, debt;
                    int creditScore, loanCount, overdueCount;
                    
                    cout << "请输入客户姓名: ";
                    cin >> name;
                    cout << "请输入身份证号: ";
                    cin >> idCard;
                    cout << "请输入月收入: ";
                    cin >> income;
                    cout << "请输入总资产: ";
                    cin >> assets;
                    cout << "请输入总负债: ";
                    cin >> debt;
                    cout << "请输入征信分数: ";
                    cin >> creditScore;
                    cout << "请输入贷款次数: ";
                    cin >> loanCount;
                    cout << "请输入逾期次数: ";
                    cin >> overdueCount;
                    
                    string cusId = riskAssessment.addCustomer(name, idCard, income, 
                                                              assets, debt, creditScore,
                                                              loanCount, overdueCount);
                    cout << "添加成功！客户ID: " << cusId << endl;
                    
                    CustomerData* c = riskAssessment.getCustomer(cusId);
                    cout << "信用等级: " << creditLevelToString(c->creditLevel) << endl;
                    cout << "风险分数: " << c->riskScore << endl;
                    cout << "高风险标记: " << (c->isHighRisk ? "是" : "否") << endl;
                    break;
                }
                case 2: {
                    string cusId;
                    cout << "请输入客户ID: ";
                    cin >> cusId;
                    
                    CustomerData* c = riskAssessment.getCustomer(cusId);
                    if (!c) {
                        cout << "客户不存在！" << endl;
                        break;
                    }
                    
                    cout << "当前信息:" << endl;
                    cout << "月收入: " << c->monthlyIncome << endl;
                    cout << "总资产: " << c->totalAssets << endl;
                    cout << "总负债: " << c->totalDebt << endl;
                    cout << "征信分数: " << c->creditScore << endl;
                    cout << "贷款次数: " << c->loanCount << endl;
                    cout << "逾期次数: " << c->overdueCount << endl;
                    
                    double income = -1, assets = -1, debt = -1;
                    int creditScore = -1, loanCount = -1, overdueCount = -1;
                    
                    cout << "\n输入新值（不修改请输入-1）:" << endl;
                    cout << "月收入: ";
                    cin >> income;
                    cout << "总资产: ";
                    cin >> assets;
                    cout << "总负债: ";
                    cin >> debt;
                    cout << "征信分数: ";
                    cin >> creditScore;
                    cout << "贷款次数: ";
                    cin >> loanCount;
                    cout << "逾期次数: ";
                    cin >> overdueCount;
                    
                    if (riskAssessment.updateCustomer(cusId, income, assets, debt,
                                                       creditScore, loanCount, overdueCount)) {
                        cout << "更新成功！" << endl;
                        c = riskAssessment.getCustomer(cusId);
                        cout << "新信用等级: " << creditLevelToString(c->creditLevel) << endl;
                    } else {
                        cout << "更新失败！" << endl;
                    }
                    break;
                }
                case 3: {
                    string cusId;
                    cout << "请输入客户ID: ";
                    cin >> cusId;
                    if (riskAssessment.deleteCustomer(cusId)) {
                        cout << "删除成功！" << endl;
                    } else {
                        cout << "删除失败！" << endl;
                    }
                    break;
                }
                case 4: {
                    string cusId;
                    cout << "请输入客户ID: ";
                    cin >> cusId;
                    riskAssessment.displayCustomer(cusId);
                    break;
                }
                case 5:
                    riskAssessment.displayAllCustomers();
                    break;
                case 6: {
                    string cusId;
                    cout << "请输入客户ID: ";
                    cin >> cusId;
                    riskAssessment.assessCustomer(cusId);
                    break;
                }
                case 7:
                    riskAssessment.assessAllCustomers();
                    break;
                case 8:
                    riskAssessment.displayHighRiskCustomers();
                    break;
                case 9: {
                    cout << "请选择信用等级 (1-AAA, 2-AA, 3-A, 4-B, 5-C): ";
                    int levelChoice;
                    cin >> levelChoice;
                    CreditLevel level = static_cast<CreditLevel>(levelChoice - 1);
                    
                    vector<CustomerData> customers = riskAssessment.getByCreditLevel(level);
                    if (customers.empty()) {
                        cout << "该等级暂无客户" << endl;
                    } else {
                        cout << "\n" << creditLevelToString(level) << " 级客户列表:" << endl;
                        for (const auto& c : customers) {
                            cout << c.customerId << " - " << c.customerName << endl;
                        }
                    }
                    break;
                }
                case 10:
                    riskAssessment.adjustRules();
                    break;
                case 11:
                    riskAssessment.displayRules();
                    break;
                case 12:
                    riskAssessment.displayStatistics();
                    break;
                case 0:
                    return;
                default:
                    cout << "无效选择！" << endl;
            }
            pause();
        }
    }
    
public:
    void run() {
        cout << "正在初始化银行综合管理系统..." << endl;
        cout << "系统加载完成！" << endl;
        
        while (true) {
            showMainMenu();
            
            int choice;
            cin >> choice;
            
            switch (choice) {
                case 1:
                    queueMenu();
                    break;
                case 2:
                    accountMenu();
                    break;
                case 3:
                    transactionMenu();
                    break;
                case 4:
                    riskMenu();
                    break;
                case 0:
                    cout << "\n感谢使用，再见！" << endl;
                    return;
                default:
                    cout << "无效选择，请重试！" << endl;
            }
        }
    }
};

int main() {
    BankSystem system;
    system.run();
    return 0;
}
