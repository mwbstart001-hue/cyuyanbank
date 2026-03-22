#include "../include/queue_system.h"
#include "../include/union_find.h"
#include "../include/huffman_tree.h"
#include "../include/decision_tree.h"
#include <cassert>

// 测试排队叫号系统
void testQueueSystem() {
    cout << "\n========== 测试排队叫号系统 ==========" << endl;
    
    QueueSystem qs(10, 3);
    
    // 测试取号
    int n1 = qs.takeNumber(NORMAL);
    int n2 = qs.takeNumber(VIP);
    int n3 = qs.takeNumber(NORMAL);
    assert(n1 == 1 && n2 == 2 && n3 == 3);
    cout << "[通过] 取号功能测试" << endl;
    
    // 测试叫号 (VIP优先，所以2号会被叫)
    assert(qs.callNumber(1) == true);
    cout << "[通过] 叫号功能测试" << endl;
    
    // 测试排队位置查询
    int pos = qs.getQueuePosition(n3);
    assert(pos > 0);
    cout << "[通过] 排队位置查询测试" << endl;
    
    // 测试普通转VIP
    assert(qs.upgradeToVip(n3) == true);
    cout << "[通过] 普通转VIP测试" << endl;
    
    // 测试完成服务 (完成VIP客户2的服务)
    assert(qs.completeService(n2) == true);
    cout << "[通过] 完成服务测试" << endl;
    
    // 测试超时检查
    qs.checkTimeout();
    cout << "[通过] 超时检查测试" << endl;
    
    cout << "排队叫号系统测试完成！\n" << endl;
}

// 测试账户关系系统
void testUnionFindSystem() {
    cout << "\n========== 测试账户关系系统 ==========" << endl;
    
    UnionFindSystem ufs(100);
    
    // 测试添加账户
    assert(ufs.addAccount(1, "张三", 10000) == true);
    assert(ufs.addAccount(2, "李四", 20000) == true);
    assert(ufs.addAccount(3, "王五", 30000) == true);
    cout << "[通过] 添加账户测试" << endl;
    
    // 测试绑定关系
    assert(ufs.bindRelation(1, 2, "亲属") == true);
    cout << "[通过] 绑定关系测试" << endl;
    
    // 测试关联查询
    assert(ufs.isRelated(1, 2) == true);
    assert(ufs.isRelated(1, 3) == false);
    cout << "[通过] 关联查询测试" << endl;
    
    // 测试解除关系
    assert(ufs.unbindRelation(1, 2, "解除关联") == true);
    cout << "[通过] 解除关系测试" << endl;
    
    // 测试删除前校验
    assert(ufs.canDeleteAccount(1) == true);
    cout << "[通过] 删除前校验测试" << endl;
    
    // 测试注销账户
    assert(ufs.removeAccount(3) == true);
    cout << "[通过] 注销账户测试" << endl;
    
    cout << "账户关系系统测试完成！\n" << endl;
}

// 测试交易流水系统
void testHuffmanSystem() {
    cout << "\n========== 测试交易流水系统 ==========" << endl;
    
    HuffmanSystem hs(100);
    
    // 测试添加交易
    int t1 = hs.addTransaction(1, "存款", 5000, "工资存入");
    int t2 = hs.addTransaction(1, "取款", 2000, "日常开销");
    int t3 = hs.addTransaction(2, "转账", 10000, "还款");
    assert(t1 > 0 && t2 > 0 && t3 > 0);
    cout << "[通过] 添加交易测试" << endl;
    
    // 测试查询交易
    Transaction* trans = hs.getTransaction(t1);
    assert(trans != nullptr && trans->amount == 5000);
    cout << "[通过] 查询交易测试" << endl;
    
    // 测试更新交易
    assert(hs.updateTransaction(t1, "description", "奖金存入") == true);
    cout << "[通过] 更新交易测试" << endl;
    
    // 测试标记删除
    assert(hs.deleteTransaction(t2) == true);
    cout << "[通过] 标记删除测试" << endl;
    
    // 测试排序
    hs.sortByTime();
    hs.sortByAmount();
    cout << "[通过] 排序功能测试" << endl;
    
    // 测试哈夫曼压缩
    assert(hs.compressTransactions("test_compress.huf") == true);
    cout << "[通过] 哈夫曼压缩测试" << endl;
    
    // 测试哈夫曼解压
    assert(hs.decompressTransactions("test_compress.huf") == true);
    cout << "[通过] 哈夫曼解压测试" << endl;
    
    cout << "交易流水系统测试完成！\n" << endl;
}

// 测试风险评估系统
void testDecisionTreeSystem() {
    cout << "\n========== 测试风险评估系统 ==========" << endl;
    
    DecisionTreeSystem dts(100);
    
    // 测试添加客户
    assert(dts.addCustomer(1, "客户A", 80000, 750, 0.3, 5, false) == true);
    assert(dts.addCustomer(2, "客户B", 30000, 600, 0.7, 1, true) == true);
    assert(dts.addCustomer(3, "客户C", 60000, 680, 0.4, 3, false) == true);
    cout << "[通过] 添加客户测试" << endl;
    
    // 测试更新客户
    assert(dts.updateCustomer(1, "income", 90000) == true);
    cout << "[通过] 更新客户测试" << endl;
    
    // 测试评估客户
    EvaluationResult r1 = dts.evaluateCustomer(1);
    EvaluationResult r2 = dts.evaluateCustomer(2);
    assert(r1.riskScore > r2.riskScore); // 客户A风险分应高于客户B
    cout << "[通过] 评估客户测试" << endl;
    
    // 测试高风险标记
    assert(r2.isHighRisk == true); // 客户B应有高风险标记
    cout << "[通过] 高风险标记测试" << endl;
    
    // 测试规则调整
    dts.adjustRules(60000, 650, 0.5, 2);
    cout << "[通过] 规则调整测试" << endl;
    
    // 测试排序
    dts.sortByCreditLevel();
    dts.sortByRiskScore();
    cout << "[通过] 排序功能测试" << endl;
    
    cout << "风险评估系统测试完成！\n" << endl;
}

// 集成测试
void testIntegration() {
    cout << "\n========== 集成测试 ==========" << endl;
    
    // 模拟完整业务流程
    QueueSystem qs(10, 3);
    UnionFindSystem ufs(100);
    HuffmanSystem hs(100);
    DecisionTreeSystem dts(100);
    
    // 1. 客户取号
    int ticket = qs.takeNumber(NORMAL);
    cout << "客户取号: " << ticket << endl;
    
    // 2. 叫号服务
    qs.callNumber(1);
    
    // 3. 开户
    ufs.addAccount(1001, "测试客户", 50000);
    
    // 4. 添加交易
    hs.addTransaction(1001, "开户", 50000, "初始存款");
    
    // 5. 录入客户信息并评估
    dts.addCustomer(1001, "测试客户", 50000, 700, 0.3, 3, false);
    EvaluationResult result = dts.evaluateCustomer(1001);
    
    cout << "业务流程完成！客户信用等级: ";
    switch (result.level) {
        case AAA: cout << "AAA"; break;
        case AA: cout << "AA"; break;
        case A: cout << "A"; break;
        case B: cout << "B"; break;
        case C: cout << "C"; break;
    }
    cout << ", 风险分: " << result.riskScore << endl;
    
    // 6. 完成服务
    qs.completeService(ticket);
    
    cout << "[通过] 集成业务流程测试" << endl;
    cout << "集成测试完成！\n" << endl;
}

// 性能测试
void testPerformance() {
    cout << "\n========== 性能测试 ==========" << endl;
    
    // 测试大数据量排序性能
    const int TEST_SIZE = 1000;
    
    HuffmanSystem hs(TEST_SIZE + 10);
    
    clock_t start = clock();
    
    // 批量添加交易
    for (int i = 0; i < TEST_SIZE; i++) {
        hs.addTransaction(i % 100, "存款", i * 100, "性能测试");
    }
    
    clock_t addEnd = clock();
    cout << "添加 " << TEST_SIZE << " 条交易耗时: " 
         << (double)(addEnd - start) / CLOCKS_PER_SEC * 1000 << " ms" << endl;
    
    // 排序测试
    hs.sortByAmount();
    clock_t sortEnd = clock();
    cout << "排序 " << TEST_SIZE << " 条交易耗时: " 
         << (double)(sortEnd - addEnd) / CLOCKS_PER_SEC * 1000 << " ms" << endl;
    
    // 压缩测试
    hs.compressTransactions("perf_test.huf");
    clock_t compressEnd = clock();
    cout << "压缩 " << TEST_SIZE << " 条交易耗时: " 
         << (double)(compressEnd - sortEnd) / CLOCKS_PER_SEC * 1000 << " ms" << endl;
    
    cout << "性能测试完成！\n" << endl;
}

int main() {
    cout << "╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "║           银行综合管理系统 - 测试套件                     ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════╝" << endl;
    
    // 运行各模块单元测试
    testQueueSystem();
    testUnionFindSystem();
    testHuffmanSystem();
    testDecisionTreeSystem();
    
    // 运行集成测试
    testIntegration();
    
    // 运行性能测试
    testPerformance();
    
    cout << "\n========================================" << endl;
    cout << "所有测试执行完毕！" << endl;
    cout << "========================================" << endl;
    
    return 0;
}
