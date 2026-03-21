#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include "CircularQueue.h"
#include "UnionFind.h"
#include "Huffman.h"
#include "DecisionTree.h"
#include "DatabaseManager.h"

using namespace std;

// 清理输入流（防止因为输入非法字符导致死循环）
void clearInput() {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// 颜色代码（用于美化控制台，仅在支持的终端有效）
const string RESET = "\033[0m";
const string GREEN = "\033[1;32m";
const string RED   = "\033[1;31m";
const string YELLOW = "\033[1;33m";
const string BLUE  = "\033[1;34m";
const string CYAN  = "\033[1;36m";

// 格式化输出账户信息
void printAccounts(const vector<Account>& accounts) {
    if (accounts.empty()) {
        cout << "当前没有任何账户。\n";
        return;
    }
    cout << "\n============================== 账户列表 ==============================\n";
    cout << left << setw(8) << "账户ID" 
         << setw(16) << "客户姓名" 
         << setw(15) << "账户余额(元)" 
         << setw(10) << "信用分" 
         << setw(10) << "状态"
         << "风险评估等级" << endl;
    cout << string(90, '-') << endl;
    for (const auto& acc : accounts) {
        string statusStr = (acc.status == AccountStatus::ACTIVE) ? "正常" : "已注销";
        cout << left << setw(8) << acc.id 
             << setw(16) << acc.name 
             << setw(15) << fixed << setprecision(2) << acc.balance 
             << setw(10) << acc.creditScore
             << setw(10) << statusStr
             << RiskAssessment::evaluate(acc.balance, acc.creditScore) << endl;
    }
    cout << string(90, '-') << endl;
}

// 打印五人分工说明
void printTeamRoles() {
    cout << "\n**************************************************************\n";
    cout << "*               银行管理系统 (零基础教学重构版)              *\n";
    cout << "**************************************************************\n";
    cout << "*  小组五人开发分工：                                        *\n";
    cout << "*  [开发者A] 核心架构与DB：DatabaseManager (SQLite封装)      *\n";
    cout << "*  [开发者B] 业务调度与交互：main.cpp (主循环与防呆处理)     *\n";
    cout << "*  [开发者C] 数据结构应用：CircularQueue排队 & Huffman压缩   *\n";
    cout << "*  [开发者D] 算法应用：UnionFind (并查集处理账户关联网络)    *\n";
    cout << "*  [开发者E] 业务模型：DecisionTree (风险评估与业务拦截)     *\n";
    cout << "**************************************************************\n\n";
}

int main() {
    printTeamRoles();

    try {
        DatabaseManager db("data/bank.db");
        CircularQueue<string> queue(10); // 排队叫号，容量10
        UnionFind uf;                    // 改用无参数构造（内部已改成 map）
        Huffman huffman;
        string transactionLogs = "";

        int choice = -1;
        while (true) {
            cout << "\n" << BLUE << "==================== 银行管理系统核心菜单 ====================" << RESET << endl;
            cout << " 1. 📂 " << CYAN << "客户取号" << RESET << " (排队系统 CircularQueue)" << endl;
            cout << " 2. 📢 " << CYAN << "柜台叫号" << RESET << " (排队系统 CircularQueue)" << endl;
            cout << " 3. 🆕 " << GREEN << "开户" << RESET << " (数据库增 + 逻辑防呆校验)" << endl;
            cout << " 4. 🔍 " << YELLOW << "查询账户" << RESET << " (数据库查 + STL回调排序)" << endl;
            cout << " 5. ❌ " << RED << "销户" << RESET << " (数据库删 + 存在性与余额校验)" << endl;
            cout << " 6. 💸 " << GREEN << "转账" << RESET << " (事务原子性 + 风险模型拦截)" << endl;
            cout << " 7. 🔗 " << CYAN << "账户关联分析" << RESET << " (并查集 UnionFind 追溯)" << endl;
            cout << " 8. 🛡️ " << BLUE << "风险等级评估" << RESET << " (决策树 DecisionTree 详报)" << endl;
            cout << " 9. 📦 " << YELLOW << "流水日志压缩" << RESET << " (哈夫曼树 Huffman 压缩演示)" << endl;
            cout << " 0. 🚪 " << RED << "妥善安全退出" << RESET << endl;
            cout << BLUE << "==============================================================" << RESET << endl;
            cout << "👉 请输入您的指令 [0-9]: ";
            
            if (!(cin >> choice)) {
                clearInput();
                cout << RED << "\n【输入错误】检测到非法字符！请输入 0-9 之间的数字序号。" << RESET << endl;
                continue;
            }

            if (choice == 0) {
                cout << "感谢使用银行管理系统，再见！" << endl;
                break;
            }

            switch (choice) {
                case 1: { // 排队
                    string name;
                    cout << "请输入排队客户姓名: ";
                    cin >> name;
                    if (queue.enqueue(name)) {
                        cout << "▶ 取号成功！客户 [" << name << "] 当前排队人数: " << queue.size() << endl;
                    } else {
                        cout << "【警告】排队人数已达上限，请稍后再试！" << endl;
                    }
                    break;
                }
                case 2: { // 叫号
                    try {
                        string name = queue.dequeue();
                        cout << "▶ 请客户 📢 [ " << name << " ] 到窗口办理业务！" << endl;
                    } catch (const exception& e) {
                        cout << "【提示】当前无人排队。" << endl;
                    }
                    break;
                }
                case 3: { // 开户
                    string name;
                    double balance;
                    int score;
                    cout << "--- 办理开户 ---" << endl;
                    cout << "输入客户姓名: ";
                    cin >> name;
                    cout << "输入初始余额 (>=0): ";
                    if (!(cin >> balance) || balance < 0) {
                        cout << "【错误】金额必须是非负数！开户中断。" << endl;
                        clearInput();
                        break;
                    }
                    cout << "输入初始信用分 (0-1000): ";
                    if (!(cin >> score) || score < 0 || score > 1000) {
                        cout << "【错误】信用分必须在0到1000之间！开户中断。" << endl;
                        clearInput();
                        break;
                    }

                    try {
                        int newId = db.addAccount(name, balance, score);
                        cout << "▶ 开户成功！分配的账户 ID 为: " << newId << endl;
                        
                        // 记录日志，格式避免包含空格等复杂字符以简化哈夫曼演示
                        transactionLogs += "C_ID" + to_string(newId) + ";";
                    } catch (const exception& e) {
                        cout << "【系统错误】" << e.what() << endl;
                    }
                    break;
                }
                case 4: { // 查询+排序
                    try {
                        auto accounts = db.getAllAccounts();
                        if (accounts.empty()) {
                            cout << "【提示】系统当前无账户记录。" << endl;
                            break;
                        }

                        cout << "请选择排序方式: \n"
                             << "  1. 按账户 ID 升序 (默认)\n"
                             << "  2. 按账户余额从高到低\n"
                             << "  3. 按风险评估从安全到高风险 (优先推荐)\n> ";
                        int sortType;
                        if (!(cin >> sortType)) {
                            clearInput();
                            sortType = 1;
                        }

                        if (sortType == 2) {
                            sort(accounts.begin(), accounts.end(), [](const Account& a, const Account& b) {
                                return a.balance > b.balance;
                            });
                        } else if (sortType == 3) {
                            sort(accounts.begin(), accounts.end(), [](const Account& a, const Account& b) {
                                int rA = RiskAssessment::getRiskLevel(a.balance, a.creditScore);
                                int rB = RiskAssessment::getRiskLevel(b.balance, b.creditScore);
                                if (rA != rB) return rA < rB; // 风险小的排前面
                                return a.balance > b.balance; // 风险相同按余额排
                            });
                        } else {
                            // 默认按 ID 排序
                            sort(accounts.begin(), accounts.end(), [](const Account& a, const Account& b) {
                                return a.id < b.id;
                            });
                        }
                        printAccounts(accounts);
                    } catch (const exception& e) {
                        cout << "【系统错误】" << e.what() << endl;
                    }
                    break;
                }
                case 5: { // 销户 (受保护，不存在的不会再被销)
                    cout << "--- 办理销户 ---" << endl;
                    int id;
                    cout << "输入要注销的账户 ID: ";
                    if (!(cin >> id)) {
                        cout << "【错误】输入无效的 ID。" << endl;
                        clearInput();
                        break;
                    }
                    try {
                        // 如果不存在，db.deleteAccount 内部会抛出异常，不再静默忽略
                        db.deleteAccount(id);
                        cout << "▶ 账户 ID [" << id << "] 注销成功！" << endl;
                        transactionLogs += "D_ID" + to_string(id) + ";";
                    } catch (const exception& e) {
                        cout << "【业务拦截】" << e.what() << " 操作取消。" << endl;
                    }
                    break;
                }
                case 6: { // 转账 (包含极强的校验机制与风险拦截)
                    cout << "--- 办理转账 ---" << endl;
                    int fromId, toId;
                    double amount;
                    
                    cout << "转出账户 ID: "; 
                    if (!(cin >> fromId)) { clearInput(); cout << "输入无效!\n"; break; }
                    cout << "转入账户 ID: ";
                    if (!(cin >> toId)) { clearInput(); cout << "输入无效!\n"; break; }
                    cout << "转账金额: ";
                    if (!(cin >> amount) || amount <= 0) { 
                        clearInput(); 
                        cout << "【错误】转账金额必须是一个大于0的有效数字!\n"; 
                        break; 
                    }

                    try {
                        // 1. 获取转出账户详情，进行风险审查
                        Account fromAcc = db.getAccount(fromId);
                        
                        // 2. 风险拦截策略：D 极高风险客户，单笔转账被限制在 5000 以内
                        if (RiskAssessment::isHighRisk(fromAcc.balance, fromAcc.creditScore)) {
                            cout << "⚠ 【风控拦截】此账户 (" << fromId << ") 属于高风险客户！\n";
                            if (amount > 5000.0) {
                                cout << "⚠ 【风控拒绝】因信用风险，拒绝大于 5000 元的转账请求。当前请求：" << amount << " 元\n";
                                // 惩罚机制：违规尝试扣除 10 分信用分
                                int newScore = std::max(0, fromAcc.creditScore - 10);
                                db.updateCreditScore(fromId, newScore);
                                cout << "⚠ 【风控惩罚】已扣除该账户 10 分信用分作为惩罚，当前信用分: " << newScore << "\n";
                                break;
                            } else {
                                cout << "⚠ 【风控警告】正在进行合规限额内的小额转账...\n";
                            }
                        }

                        // 3. 执行数据库转账（包含余额校验和事务保护，详见 DatabaseManager::transfer）
                        db.transfer(fromId, toId, amount);

                        // 4. 重构并查集关联关系：记录两个账户产生过资金往来
                        uf.unite(fromId, toId);

                        cout << "▶ 转账办理成功！" << amount << " 元已从 ID:" << fromId << " 转移至 ID:" << toId << endl;
                        
                        // 5. 记录日志
                        transactionLogs += "T_" + to_string(fromId) + "_" + to_string(toId) + ";";

                    } catch (const exception& e) {
                        cout << "【转账失败】" << e.what() << endl;
                    }
                    break;
                }
                case 7: { // 检查关联 (由 UnionFind 提供强大的背后支持)
                    cout << "--- 银行反洗钱网络/风险联查 ---" << endl;
                    int id1, id2;
                    cout << "输入要核查关联的两个账户 ID (空格分隔): ";
                    if (!(cin >> id1 >> id2)) {
                        clearInput();
                        cout << "输入无效！\n";
                        break;
                    }
                    
                    if (!db.accountExists(id1) || !db.accountExists(id2)) {
                        cout << "【提示】输入的账户不存在（可能含已注销或未创建）。这不影响关联追溯。" << endl;
                    }
                    if (uf.isConnected(id1, id2)) {
                        cout << "🚨 [红灯警报] 系统检测到 ID:" << id1 << " 与 ID:" << id2 
                             << " 存在直接或间接的资金往来，已被关联！" << endl;
                    } else {
                        cout << "✅ [绿灯安全] 这两个账户目前没有任何资金交叉。" << endl;
                    }
                    break;
                }
                case 8: { // 风险评估详报（新增功能点，满足用户提到的“数据校验与风险评估”）
                    int id;
                    cout << "请输入要进行深度风险评估的账户 ID: ";
                    if (!(cin >> id)) { clearInput(); break; }
                    try {
                        Account acc = db.getAccount(id);
                        cout << RiskAssessment::getDetailedReport(acc.balance, acc.creditScore) << endl;
                    } catch (const exception& e) {
                        cout << RED << "【评估失败】" << e.what() << RESET << endl;
                    }
                    break;
                }
                case 9: { // 日志压缩与解压闭环展示
                    cout << YELLOW << "--- 交易日志流压缩/解压缩演示 ---" << RESET << endl;
                    if (transactionLogs.empty()) {
                        cout << "【提示】当前系统还没有产生任何活跃交易日志。" << endl;
                    } else {
                        huffman.build(transactionLogs);
                        string compressed = huffman.compress(transactionLogs);
                        string decompressed = huffman.decompress(compressed);

                        cout << "\n[1] 原始日志: " << transactionLogs << " (" << transactionLogs.length() << " 字符)" << endl;
                        cout << "[2] 压缩后位流: " << compressed << " (" << compressed.length() << " bits)" << endl;
                        double ratio = 100.0 * (1.0 - (double)compressed.length() / (transactionLogs.length() * 8));
                        cout << "    ▶ " << GREEN << "节省空间: " << fixed << setprecision(2) << ratio << "%" << RESET << endl;
                        cout << "[3] 还原内容: " << decompressed << endl;
                        if (decompressed == transactionLogs) cout << GREEN << "    ▶ 校验成功: 无损解压成功！" << RESET << endl;
                    }
                    break;
                }
                default:
                    cout << YELLOW << "【提示】无效选项，请输入 0-9 之间的数字。" << RESET << endl;
            }
        }
    } catch (const exception& e) {
        cerr << "【致命错误】系统崩溃: " << e.what() << endl;
    }

    return 0;
}
