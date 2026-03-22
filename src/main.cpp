#include "common.h"
#include "queuing_system.h"
#include "account_relationship.h"
#include "transaction_flow.h"
#include "risk_assessment.h"
#include "file_storage.h"

class BankManagementSystem {
private:
    QueuingSystem queuingSystem;
    AccountRelationshipModule accountModule;
    TransactionFlowModule transactionModule;
    RiskAssessmentModule riskModule;
    FileStorage fileStorage;

    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    void pause() {
        cout << "\nPress Enter to continue...";
        cin.ignore();
        cin.get();
    }

    void printHeader(const string& title) {
        clearScreen();
        cout << "========================================" << endl;
        cout << "   " << title << endl;
        cout << "========================================" << endl;
    }

    string customerTypeToString(CustomerType type) {
        return type == VIP ? "VIP" : "普通";
    }

    string creditRatingToString(CreditRating rating) {
        switch (rating) {
            case AAA: return "AAA";
            case AA: return "AA";
            case A: return "A";
            case B: return "B";
            case C: return "C";
            default: return "未知";
        }
    }

public:
    BankManagementSystem() {
        fileStorage.loadAllData(queuingSystem, accountModule, transactionModule, riskModule);
    }

    ~BankManagementSystem() {
        fileStorage.saveAllData(queuingSystem, accountModule, transactionModule, riskModule);
    }

    void showMainMenu() {
        while (true) {
            printHeader("银行综合管理系统");
            cout << "1. 排队叫号系统" << endl;
            cout << "2. 账户关系管理" << endl;
            cout << "3. 交易流水管理" << endl;
            cout << "4. 风险评估系统" << endl;
            cout << "5. 数据存储与备份" << endl;
            cout << "6. 系统测试" << endl;
            cout << "0. 退出系统" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: showQueuingMenu(); break;
                case 2: showAccountMenu(); break;
                case 3: showTransactionMenu(); break;
                case 4: showRiskMenu(); break;
                case 5: showStorageMenu(); break;
                case 6: runTests(); break;
                case 0: cout << "感谢使用银行管理系统！" << endl; return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void showQueuingMenu() {
        while (true) {
            printHeader("排队叫号系统");
            cout << "当前排队人数: " << queuingSystem.getCurrentQueueSize() << endl;
            cout << "普通客户: " << queuingSystem.getNormalCustomerCount()
                 << "  VIP客户: " << queuingSystem.getVIPCustomerCount() << endl;
            cout << "========================================" << endl;
            cout << "1. 取号" << endl;
            cout << "2. 叫号" << endl;
            cout << "3. 查询排队位置" << endl;
            cout << "4. 普通客户升级为VIP" << endl;
            cout << "5. 取消排队" << endl;
            cout << "6. 查看排队列表" << endl;
            cout << "7. 清理过期号码" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: takeNumber(); break;
                case 2: callNumber(); break;
                case 3: queryPosition(); break;
                case 4: upgradeToVIP(); break;
                case 5: cancelNumber(); break;
                case 6: showQueueList(); break;
                case 7: cleanExpired(); break;
                case 0: return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void takeNumber() {
        printHeader("取号");

        string name, idCard;
        int typeChoice;

        cout << "请输入姓名: ";
        cin >> name;
        cout << "请输入身份证号: ";
        cin >> idCard;
        cout << "请选择客户类型 (1-普通, 2-VIP): ";
        cin >> typeChoice;

        CustomerType type = (typeChoice == 2) ? VIP : NORMAL;
        int ticketId = queuingSystem.takeNumber(name, idCard, type);

        if (ticketId > 0) {
            cout << "\n取号成功！" << endl;
            cout << "您的号码是: " << ticketId << endl;
            cout << "客户类型: " << customerTypeToString(type) << endl;
            cout << "当前排队位置: " << queuingSystem.getQueuePosition(ticketId) << endl;
        } else {
            cout << "\n取号失败，队列已满！" << endl;
        }
        pause();
    }

    void callNumber() {
        printHeader("叫号");

        int windowNumber;
        cout << "请输入窗口号 (1-5): ";
        cin >> windowNumber;

        Customer customer;
        if (queuingSystem.callNumber(windowNumber, customer)) {
            cout << "\n叫号成功！" << endl;
            cout << "号码: " << customer.id << endl;
            cout << "姓名: " << customer.name << endl;
            cout << "客户类型: " << customerTypeToString(customer.type) << endl;
            cout << "请到 " << windowNumber << " 号窗口办理业务！" << endl;
        } else {
            cout << "\n叫号失败，当前没有等待的客户或窗口繁忙！" << endl;
        }
        pause();
    }

    void queryPosition() {
        printHeader("查询排队位置");

        int ticketId;
        cout << "请输入您的号码: ";
        cin >> ticketId;

        int position = queuingSystem.getQueuePosition(ticketId);
        if (position > 0) {
            cout << "\n您的当前排队位置是: " << position << endl;
        } else {
            cout << "\n未找到该号码或号码已过期！" << endl;
        }
        pause();
    }

    void upgradeToVIP() {
        printHeader("普通客户升级为VIP");

        int ticketId;
        cout << "请输入要升级的号码: ";
        cin >> ticketId;

        if (queuingSystem.upgradeToVIP(ticketId)) {
            cout << "\n升级成功！客户已升级为VIP客户！" << endl;
            cout << "新的排队位置: " << queuingSystem.getQueuePosition(ticketId) << endl;
        } else {
            cout << "\n升级失败！未找到该号码或该客户已是VIP！" << endl;
        }
        pause();
    }

    void cancelNumber() {
        printHeader("取消排队");

        int ticketId;
        cout << "请输入要取消的号码: ";
        cin >> ticketId;

        if (queuingSystem.cancelNumber(ticketId)) {
            cout << "\n取消排队成功！" << endl;
        } else {
            cout << "\n取消失败！未找到该号码！" << endl;
        }
        pause();
    }

    void showQueueList() {
        printHeader("排队列表");

        vector<Customer> list = queuingSystem.getSortedByPriority();

        if (list.empty()) {
            cout << "当前队列为空！" << endl;
        } else {
            cout << "位置\t号码\t姓名\t类型\t取号时间" << endl;
            cout << "----------------------------------------" << endl;
            for (size_t i = 0; i < list.size(); i++) {
                cout << i + 1 << "\t"
                     << list[i].id << "\t"
                     << list[i].name << "\t"
                     << customerTypeToString(list[i].type) << "\t"
                     << timeToString(list[i].createTime) << endl;
            }
        }
        pause();
    }

    void cleanExpired() {
        printHeader("清理过期号码");

        queuingSystem.cleanExpired();
        cout << "清理完成！当前排队人数: " << queuingSystem.getCurrentQueueSize() << endl;
        pause();
    }

    void showAccountMenu() {
        while (true) {
            printHeader("账户关系管理");
            cout << "总账户数: " << accountModule.getTotalAccounts()
                 << "  活跃账户: " << accountModule.getActiveAccounts() << endl;
            cout << "总余额: " << accountModule.getTotalBalance() << endl;
            cout << "========================================" << endl;
            cout << "1. 新建账户" << endl;
            cout << "2. 注销账户" << endl;
            cout << "3. 查询账户信息" << endl;
            cout << "4. 绑定账户关系" << endl;
            cout << "5. 解除账户关系" << endl;
            cout << "6. 查询关联账户" << endl;
            cout << "7. 查看所有账户" << endl;
            cout << "8. 修改组备注" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: createAccount(); break;
                case 2: deleteAccount(); break;
                case 3: queryAccount(); break;
                case 4: bindRelationship(); break;
                case 5: unbindRelationship(); break;
                case 6: queryRelatedAccounts(); break;
                case 7: showAllAccounts(); break;
                case 8: updateGroupRemark(); break;
                case 0: return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void createAccount() {
        printHeader("新建账户");

        string accountId, name, idCard;
        double initialBalance;

        cout << "请输入账号: ";
        cin >> accountId;
        cout << "请输入姓名: ";
        cin >> name;
        cout << "请输入身份证号: ";
        cin >> idCard;
        cout << "请输入初始余额: ";
        cin >> initialBalance;

        if (accountModule.createAccount(accountId, name, idCard, initialBalance)) {
            cout << "\n账户创建成功！" << endl;
        } else {
            cout << "\n账户创建失败！账号已存在！" << endl;
        }
        pause();
    }

    void deleteAccount() {
        printHeader("注销账户");

        string accountId;
        cout << "请输入要注销的账号: ";
        cin >> accountId;

        if (accountModule.deleteAccount(accountId)) {
            cout << "\n账户注销成功！" << endl;
        } else {
            cout << "\n账户注销失败！未找到该账户或存在未完成交易！" << endl;
        }
        pause();
    }

    void queryAccount() {
        printHeader("查询账户信息");

        string accountId;
        cout << "请输入账号: ";
        cin >> accountId;

        Account* acc = accountModule.getAccount(accountId);
        if (acc) {
            cout << "\n账号: " << acc->accountId << endl;
            cout << "姓名: " << acc->customerName << endl;
            cout << "身份证号: " << acc->idCard << endl;
            cout << "余额: " << acc->balance << endl;
            cout << "开户时间: " << timeToString(acc->openTime) << endl;
            cout << "状态: " << (acc->status == ACTIVE ? "活跃" : "已注销") << endl;
            cout << "备注: " << acc->remark << endl;
        } else {
            cout << "\n未找到该账户！" << endl;
        }
        pause();
    }

    void bindRelationship() {
        printHeader("绑定账户关系");

        string accountId1, accountId2;
        cout << "请输入第一个账号: ";
        cin >> accountId1;
        cout << "请输入第二个账号: ";
        cin >> accountId2;

        if (accountModule.bindRelationship(accountId1, accountId2)) {
            cout << "\n账户关系绑定成功！" << endl;
        } else {
            cout << "\n绑定失败！账户不存在或已绑定！" << endl;
        }
        pause();
    }

    void unbindRelationship() {
        printHeader("解除账户关系");

        string accountId1, accountId2;
        cout << "请输入第一个账号: ";
        cin >> accountId1;
        cout << "请输入第二个账号: ";
        cin >> accountId2;

        if (accountModule.unbindRelationship(accountId1, accountId2)) {
            cout << "\n账户关系解除成功！" << endl;
        } else {
            cout << "\n解除失败！账户不存在或未绑定！" << endl;
        }
        pause();
    }

    void queryRelatedAccounts() {
        printHeader("查询关联账户");

        string accountId;
        cout << "请输入账号: ";
        cin >> accountId;

        vector<string> related = accountModule.getRelatedAccounts(accountId);
        if (related.empty()) {
            cout << "\n该账户没有关联账户！" << endl;
        } else {
            cout << "\n关联账户列表:" << endl;
            for (const string& id : related) {
                Account* acc = accountModule.getAccount(id);
                if (acc) {
                    cout << "账号: " << acc->accountId << " 姓名: " << acc->customerName << endl;
                }
            }
            string remark = accountModule.getGroupRemark(accountId);
            if (!remark.empty()) {
                cout << "组备注: " << remark << endl;
            }
        }
        pause();
    }

    void showAllAccounts() {
        printHeader("所有账户列表");

        vector<Account> accounts = accountModule.getSortedByBalance(false);

        if (accounts.empty()) {
            cout << "暂无账户信息！" << endl;
        } else {
            cout << "账号\t\t姓名\t余额\t开户时间" << endl;
            cout << "----------------------------------------" << endl;
            for (const auto& acc : accounts) {
                cout << acc.accountId << "\t"
                     << acc.customerName << "\t"
                     << acc.balance << "\t"
                     << timeToString(acc.openTime) << endl;
            }
        }
        pause();
    }

    void updateGroupRemark() {
        printHeader("修改组备注");

        string accountId, remark;
        cout << "请输入组内任意账号: ";
        cin >> accountId;
        cout << "请输入备注内容: ";
        cin.ignore();
        getline(cin, remark);

        if (accountModule.updateGroupRemark(accountId, remark)) {
            cout << "\n备注修改成功！" << endl;
        } else {
            cout << "\n修改失败！未找到该账户！" << endl;
        }
        pause();
    }

    void showTransactionMenu() {
        while (true) {
            printHeader("交易流水管理");
            cout << "交易总数: " << transactionModule.getTransactionCount() << endl;
            cout << "交易总金额: " << transactionModule.getTotalTransactionAmount() << endl;
            cout << "数据是否压缩: " << (transactionModule.isDataCompressed() ? "是" : "否") << endl;
            cout << "========================================" << endl;
            cout << "1. 添加交易记录" << endl;
            cout << "2. 删除交易记录" << endl;
            cout << "3. 修改交易记录" << endl;
            cout << "4. 查询交易记录" << endl;
            cout << "5. 查看所有交易" << endl;
            cout << "6. 压缩交易数据" << endl;
            cout << "7. 解压交易数据" << endl;
            cout << "8. 查看压缩率" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: addTransaction(); break;
                case 2: deleteTransaction(); break;
                case 3: updateTransaction(); break;
                case 4: queryTransaction(); break;
                case 5: showAllTransactions(); break;
                case 6: compressTransactions(); break;
                case 7: decompressTransactions(); break;
                case 8: showCompressionRate(); break;
                case 0: return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void addTransaction() {
        printHeader("添加交易记录");

        Transaction trans;
        int typeChoice;

        cout << "请输入交易ID: ";
        cin >> trans.transactionId;
        cout << "请输入账号: ";
        cin >> trans.accountId;
        cout << "请选择交易类型 (1-存款, 2-取款, 3-转账, 4-支付): ";
        cin >> typeChoice;
        trans.type = static_cast<TransactionType>(typeChoice - 1);
        cout << "请输入交易金额: ";
        cin >> trans.amount;
        cout << "请输入交易描述: ";
        cin.ignore();
        getline(cin, trans.description);
        cout << "请输入操作员ID: ";
        cin >> trans.operatorId;

        trans.createTime = time(nullptr);

        if (transactionModule.addTransaction(trans)) {
            cout << "\n交易记录添加成功！" << endl;
        } else {
            cout << "\n添加失败！交易ID已存在或数据过大！" << endl;
        }
        pause();
    }

    void deleteTransaction() {
        printHeader("删除交易记录");

        string transactionId;
        cout << "请输入要删除的交易ID: ";
        cin >> transactionId;

        if (transactionModule.deleteTransaction(transactionId)) {
            cout << "\n交易记录已标记删除！" << endl;
        } else {
            cout << "\n删除失败！未找到该交易记录！" << endl;
        }
        pause();
    }

    void updateTransaction() {
        printHeader("修改交易记录");

        string transactionId;
        cout << "请输入要修改的交易ID: ";
        cin >> transactionId;

        Transaction* trans = transactionModule.getTransaction(transactionId);
        if (!trans) {
            cout << "\n未找到该交易记录！" << endl;
            pause();
            return;
        }

        Transaction newTrans = *trans;
        string input;

        cout << "请输入新的交易描述 (回车不修改): ";
        cin.ignore();
        getline(cin, input);
        if (!input.empty()) newTrans.description = input;

        cout << "请输入新的交易金额 (0不修改): ";
        double amount;
        cin >> amount;
        if (amount != 0) newTrans.amount = amount;

        if (transactionModule.updateTransaction(transactionId, newTrans)) {
            cout << "\n交易记录修改成功！" << endl;
        } else {
            cout << "\n修改失败！" << endl;
        }
        pause();
    }

    void queryTransaction() {
        printHeader("查询交易记录");

        string transactionId;
        cout << "请输入交易ID: ";
        cin >> transactionId;

        Transaction* trans = transactionModule.getTransaction(transactionId);
        if (trans) {
            cout << "\n交易ID: " << trans->transactionId << endl;
            cout << "账号: " << trans->accountId << endl;
            cout << "交易类型: ";
            switch (trans->type) {
                case DEPOSIT: cout << "存款"; break;
                case WITHDRAWAL: cout << "取款"; break;
                case TRANSFER: cout << "转账"; break;
                case PAYMENT: cout << "支付"; break;
            }
            cout << endl;
            cout << "交易金额: " << trans->amount << endl;
            cout << "交易描述: " << trans->description << endl;
            cout << "交易时间: " << timeToString(trans->createTime) << endl;
            cout << "操作员: " << trans->operatorId << endl;
            cout << "状态: " << (trans->isDeleted ? "已删除" : "正常") << endl;
        } else {
            cout << "\n未找到该交易记录！" << endl;
        }
        pause();
    }

    void showAllTransactions() {
        printHeader("所有交易记录");

        vector<Transaction> transactions = transactionModule.getSortedByTime(false);

        if (transactions.empty()) {
            cout << "暂无交易记录！" << endl;
        } else {
            cout << "交易ID\t账号\t类型\t金额\t交易时间" << endl;
            cout << "----------------------------------------" << endl;
            for (const auto& trans : transactions) {
                string typeStr;
                switch (trans.type) {
                    case DEPOSIT: typeStr = "存款"; break;
                    case WITHDRAWAL: typeStr = "取款"; break;
                    case TRANSFER: typeStr = "转账"; break;
                    case PAYMENT: typeStr = "支付"; break;
                }
                cout << trans.transactionId << "\t"
                     << trans.accountId << "\t"
                     << typeStr << "\t"
                     << trans.amount << "\t"
                     << timeToString(trans.createTime) << endl;
            }
        }
        pause();
    }

    void compressTransactions() {
        printHeader("压缩交易数据");

        if (transactionModule.compressTransactions()) {
            cout << "压缩成功！" << endl;
            cout << "压缩率: " << transactionModule.getCompressionRate() << "%" << endl;
        } else {
            cout << "压缩失败！数据已压缩或为空！" << endl;
        }
        pause();
    }

    void decompressTransactions() {
        printHeader("解压交易数据");

        if (transactionModule.decompressTransactions()) {
            cout << "解压成功！" << endl;
        } else {
            cout << "解压失败！数据未压缩！" << endl;
        }
        pause();
    }

    void showCompressionRate() {
        printHeader("查看压缩率");

        cout << "当前压缩率: " << transactionModule.getCompressionRate() << "%" << endl;
        pause();
    }

    void showRiskMenu() {
        while (true) {
            printHeader("风险评估系统");
            cout << "客户总数: " << riskModule.getCustomerCount() << endl;
            cout << "高风险客户数: " << riskModule.getHighRiskCustomerCount() << endl;
            cout << "平均风险评分: " << riskModule.getAverageRiskScore() << endl;
            cout << "========================================" << endl;
            cout << "1. 录入客户数据" << endl;
            cout << "2. 评估客户风险" << endl;
            cout << "3. 批量评估" << endl;
            cout << "4. 查询评估结果" << endl;
            cout << "5. 查看高风险客户" << endl;
            cout << "6. 生成评估报告" << endl;
            cout << "7. 管理评估规则" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: addCustomerData(); break;
                case 2: assessCustomer(); break;
                case 3: batchAssessment(); break;
                case 4: queryAssessmentResult(); break;
                case 5: showHighRiskCustomers(); break;
                case 6: generateAssessmentReport(); break;
                case 7: manageAssessmentRules(); break;
                case 0: return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void addCustomerData() {
        printHeader("录入客户数据");

        CustomerData data;

        cout << "请输入客户ID: ";
        cin >> data.customerId;
        cout << "请输入姓名: ";
        cin >> data.name;
        cout << "请输入身份证号: ";
        cin >> data.idCard;
        cout << "请输入月收入: ";
        cin >> data.monthlyIncome;
        cout << "请输入信用评分 (300-850): ";
        cin >> data.creditScore;
        cout << "请输入负债率 (0-1): ";
        cin >> data.debtRatio;
        cout << "请输入年龄: ";
        cin >> data.age;
        cout << "请输入就业状态 (Employed/Self-employed/Unemployed): ";
        cin >> data.employmentStatus;

        if (riskModule.addCustomerData(data)) {
            cout << "\n客户数据录入成功！" << endl;
        } else {
            cout << "\n录入失败！客户ID已存在！" << endl;
        }
        pause();
    }

    void assessCustomer() {
        printHeader("评估客户风险");

        string customerId, assessor;
        cout << "请输入客户ID: ";
        cin >> customerId;
        cout << "请输入评估员ID: ";
        cin >> assessor;

        if (riskModule.assessCustomer(customerId, assessor)) {
            CustomerData* data = riskModule.getCustomerData(customerId);
            if (data) {
                cout << "\n评估完成！" << endl;
                cout << "风险评分: " << data->riskScore << "/100" << endl;
                cout << "信用等级: " << creditRatingToString(data->rating) << endl;
                cout << "是否高风险: " << (data->isHighRisk ? "是" : "否") << endl;
            }
        } else {
            cout << "\n评估失败！未找到该客户！" << endl;
        }
        pause();
    }

    void batchAssessment() {
        printHeader("批量评估");

        riskModule.batchAssessment();
        cout << "批量评估完成！" << endl;
        cout << "已评估客户数: " << riskModule.getCustomerCount() << endl;
        pause();
    }

    void queryAssessmentResult() {
        printHeader("查询评估结果");

        string customerId;
        cout << "请输入客户ID: ";
        cin >> customerId;

        CustomerData* data = riskModule.getCustomerData(customerId);
        if (data && data->assessmentTime > 0) {
            cout << "\n客户ID: " << data->customerId << endl;
            cout << "姓名: " << data->name << endl;
            cout << "风险评分: " << data->riskScore << "/100" << endl;
            cout << "信用等级: " << creditRatingToString(data->rating) << endl;
            cout << "是否高风险: " << (data->isHighRisk ? "是" : "否") << endl;
            cout << "评估时间: " << timeToString(data->assessmentTime) << endl;
            cout << "评估员: " << data->assessor << endl;
        } else {
            cout << "\n未找到该客户或未进行评估！" << endl;
        }
        pause();
    }

    void showHighRiskCustomers() {
        printHeader("高风险客户列表");

        vector<CustomerData> list = riskModule.getHighRiskCustomers();

        if (list.empty()) {
            cout << "暂无高风险客户！" << endl;
        } else {
            cout << "客户ID\t姓名\t信用等级\t风险评分" << endl;
            cout << "----------------------------------------" << endl;
            for (const auto& data : list) {
                cout << data.customerId << "\t"
                     << data.name << "\t"
                     << creditRatingToString(data.rating) << "\t\t"
                     << data.riskScore << endl;
            }
        }
        pause();
    }

    void generateAssessmentReport() {
        printHeader("生成评估报告");

        string customerId;
        cout << "请输入客户ID: ";
        cin >> customerId;

        string report = riskModule.generateAssessmentReport(customerId);
        cout << "\n" << report << endl;
        pause();
    }

    void manageAssessmentRules() {
        printHeader("管理评估规则");

        vector<AssessmentRule> rules = riskModule.getAssessmentRules();

        cout << "规则ID\t维度\t最小值\t最大值\t评分\t描述" << endl;
        cout << "----------------------------------------" << endl;
        for (const auto& rule : rules) {
            cout << rule.ruleId << "\t"
                 << rule.dimension << "\t"
                 << rule.minValue << "\t"
                 << rule.maxValue << "\t"
                 << rule.score << "\t"
                 << rule.description << endl;
        }
        pause();
    }

    void showStorageMenu() {
        while (true) {
            printHeader("数据存储与备份");
            cout << "========================================" << endl;
            cout << "1. 保存所有数据" << endl;
            cout << "2. 加载所有数据" << endl;
            cout << "3. 创建备份" << endl;
            cout << "4. 恢复备份" << endl;
            cout << "5. 查看备份列表" << endl;
            cout << "6. 删除备份" << endl;
            cout << "0. 返回主菜单" << endl;
            cout << "========================================" << endl;
            cout << "请输入您的选择: ";

            int choice;
            cin >> choice;

            switch (choice) {
                case 1: saveAllData(); break;
                case 2: loadAllData(); break;
                case 3: createBackup(); break;
                case 4: restoreBackup(); break;
                case 5: listBackups(); break;
                case 6: deleteBackup(); break;
                case 0: return;
                default: cout << "无效输入，请重新选择！" << endl; pause();
            }
        }
    }

    void saveAllData() {
        printHeader("保存所有数据");

        if (fileStorage.saveAllData(queuingSystem, accountModule, transactionModule, riskModule)) {
            cout << "数据保存成功！" << endl;
        } else {
            cout << "数据保存失败！" << endl;
        }
        pause();
    }

    void loadAllData() {
        printHeader("加载所有数据");

        if (fileStorage.loadAllData(queuingSystem, accountModule, transactionModule, riskModule)) {
            cout << "数据加载成功！" << endl;
        } else {
            cout << "数据加载失败！" << endl;
        }
        pause();
    }

    void createBackup() {
        printHeader("创建备份");

        string backupName;
        cout << "请输入备份名称: ";
        cin >> backupName;

        if (fileStorage.backupData(backupName)) {
            cout << "备份创建成功！" << endl;
        } else {
            cout << "备份创建失败！" << endl;
        }
        pause();
    }

    void restoreBackup() {
        printHeader("恢复备份");

        string backupName;
        cout << "请输入备份名称: ";
        cin >> backupName;

        if (fileStorage.restoreBackup(backupName)) {
            cout << "备份恢复成功！" << endl;
            fileStorage.loadAllData(queuingSystem, accountModule, transactionModule, riskModule);
        } else {
            cout << "备份恢复失败！" << endl;
        }
        pause();
    }

    void listBackups() {
        printHeader("备份列表");

        vector<string> backups = fileStorage.listBackups();

        if (backups.empty()) {
            cout << "暂无备份！" << endl;
        } else {
            cout << "备份名称:" << endl;
            for (const string& name : backups) {
                cout << "- " << name << endl;
            }
        }
        pause();
    }

    void deleteBackup() {
        printHeader("删除备份");

        string backupName;
        cout << "请输入要删除的备份名称: ";
        cin >> backupName;

        if (fileStorage.deleteBackup(backupName)) {
            cout << "备份删除成功！" << endl;
        } else {
            cout << "备份删除失败！" << endl;
        }
        pause();
    }

    void runTests() {
        printHeader("系统测试");

        cout << "正在运行测试用例..." << endl << endl;

        bool testPassed = true;

        testPassed &= testQueuingSystem();
        testPassed &= testAccountRelationship();
        testPassed &= testTransactionFlow();
        testPassed &= testRiskAssessment();
        testPassed &= testFileStorage();

        cout << endl << "========================================" << endl;
        if (testPassed) {
            cout << "所有测试用例通过！" << endl;
        } else {
            cout << "部分测试用例失败！" << endl;
        }
        cout << "========================================" << endl;
        pause();
    }

    bool testQueuingSystem() {
        cout << "[测试] 排队叫号系统..." << endl;

        QueuingSystem testQueue(10, 30, 5);

        int id1 = testQueue.takeNumber("张三", "110101199001011234", NORMAL);
        int id2 = testQueue.takeNumber("李四", "110101199001011235", VIP);
        int id3 = testQueue.takeNumber("王五", "110101199001011236", NORMAL);

        if (id1 <= 0 || id2 <= 0 || id3 <= 0) {
            cout << "  FAIL: 取号失败" << endl;
            return false;
        }

        int pos = testQueue.getQueuePosition(id2);
        if (pos != 1) {
            cout << "  FAIL: VIP客户优先级排序错误" << endl;
            return false;
        }

        if (!testQueue.upgradeToVIP(id1)) {
            cout << "  FAIL: 升级VIP失败" << endl;
            return false;
        }

        if (testQueue.getQueuePosition(id1) != 1) {
            cout << "  FAIL: 升级后位置错误" << endl;
            return false;
        }

        Customer cust;
        if (!testQueue.callNumber(1, cust) || cust.id != id1) {
            cout << "  FAIL: 叫号失败" << endl;
            return false;
        }

        if (!testQueue.cancelNumber(id3)) {
            cout << "  FAIL: 取消排队失败" << endl;
            return false;
        }

        cout << "  PASS" << endl;
        return true;
    }

    bool testAccountRelationship() {
        cout << "[测试] 账户关系管理..." << endl;

        AccountRelationshipModule testAccounts;

        if (!testAccounts.createAccount("A001", "张三", "110101199001011234", 10000)) {
            cout << "  FAIL: 创建账户失败" << endl;
            return false;
        }

        testAccounts.createAccount("A002", "李四", "110101199001011235", 20000);
        testAccounts.createAccount("A003", "王五", "110101199001011236", 30000);

        if (!testAccounts.bindRelationship("A001", "A002")) {
            cout << "  FAIL: 绑定关系失败" << endl;
            return false;
        }

        vector<string> related = testAccounts.getRelatedAccounts("A001");
        if (related.size() != 1 || related[0] != "A002") {
            cout << "  FAIL: 查询关联账户失败" << endl;
            return false;
        }

        vector<Account> sorted = testAccounts.getSortedByBalance(false);
        if (sorted[0].accountId != "A003") {
            cout << "  FAIL: 按余额排序失败" << endl;
            return false;
        }

        cout << "  PASS" << endl;
        return true;
    }

    bool testTransactionFlow() {
        cout << "[测试] 交易流水管理..." << endl;

        TransactionFlowModule testTransactions(2048);

        Transaction t1;
        t1.transactionId = "T001";
        t1.accountId = "A001";
        t1.type = DEPOSIT;
        t1.amount = 5000;
        t1.description = "存款";
        t1.createTime = time(nullptr);
        t1.operatorId = "OP001";

        if (!testTransactions.addTransaction(t1)) {
            cout << "  FAIL: 添加交易失败" << endl;
            return false;
        }

        Transaction* retrieved = testTransactions.getTransaction("T001");
        if (!retrieved || retrieved->amount != 5000) {
            cout << "  FAIL: 查询交易失败" << endl;
            return false;
        }

        double rate = testTransactions.getCompressionRate();
        if (rate < 0) {
            cout << "  FAIL: 压缩率计算失败" << endl;
            return false;
        }

        vector<Transaction> sorted = testTransactions.getSortedByAmount(false);
        if (sorted.empty() || sorted[0].transactionId != "T001") {
            cout << "  FAIL: 按金额排序失败" << endl;
            return false;
        }

        cout << "  PASS" << endl;
        return true;
    }

    bool testRiskAssessment() {
        cout << "[测试] 风险评估系统..." << endl;

        RiskAssessmentModule testRisk;

        CustomerData data;
        data.customerId = "C001";
        data.name = "测试客户";
        data.idCard = "110101199001011234";
        data.monthlyIncome = 30000;
        data.creditScore = 720;
        data.debtRatio = 0.25;
        data.age = 35;
        data.employmentStatus = "Employed";

        if (!testRisk.addCustomerData(data)) {
            cout << "  FAIL: 添加客户数据失败" << endl;
            return false;
        }

        if (!testRisk.assessCustomer("C001", "TEST")) {
            cout << "  FAIL: 风险评估失败" << endl;
            return false;
        }

        CustomerData* result = testRisk.getCustomerData("C001");
        if (!result || result->riskScore <= 0) {
            cout << "  FAIL: 评估结果无效" << endl;
            return false;
        }

        if (result->rating < AA || result->rating > A) {
            cout << "  FAIL: 信用等级评估错误" << endl;
            return false;
        }

        string report = testRisk.generateAssessmentReport("C001");
        if (report.empty()) {
            cout << "  FAIL: 生成报告失败" << endl;
            return false;
        }

        cout << "  PASS" << endl;
        return true;
    }

    bool testFileStorage() {
        cout << "[测试] 文件存储..." << endl;

        FileStorage testStorage("test_data");

        QueuingSystem queue;
        AccountRelationshipModule accounts;
        TransactionFlowModule transactions;
        RiskAssessmentModule risk;

        accounts.createAccount("TEST001", "测试", "123456", 1000);

        if (!testStorage.saveAllData(queue, accounts, transactions, risk)) {
            cout << "  FAIL: 保存数据失败" << endl;
            return false;
        }

        AccountRelationshipModule newAccounts;
        testStorage.loadAllData(queue, newAccounts, transactions, risk);

        cout << "  PASS" << endl;
        return true;
    }
};

int main() {
    srand(time(nullptr));

    BankManagementSystem system;
    system.showMainMenu();

    return 0;
}