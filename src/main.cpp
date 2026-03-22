#include "../include/queue_system.h"
#include "../include/union_find.h"
#include "../include/huffman_tree.h"
#include "../include/decision_tree.h"

void showMainMenu() {
    cout << "\n╔══════════════════════════════════════════════════════════╗" << endl;
    cout << "║           银行综合管理系统 v1.0                          ║" << endl;
    cout << "╠══════════════════════════════════════════════════════════╣" << endl;
    cout << "║  1. 排队叫号系统 (循环队列)                              ║" << endl;
    cout << "║  2. 账户关系系统 (并查集)                                ║" << endl;
    cout << "║  3. 交易流水系统 (哈夫曼树压缩)                          ║" << endl;
    cout << "║  4. 风险评估系统 (决策树)                                ║" << endl;
    cout << "║  0. 退出系统                                             ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════╝" << endl;
    cout << "请选择功能模块: ";
}

int main() {
    QueueSystem queueSystem;
    UnionFindSystem unionFindSystem;
    HuffmanSystem huffmanSystem;
    DecisionTreeSystem decisionTreeSystem;
    
    int choice;
    
    do {
        showMainMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                queueSystem.run();
                break;
            }
            case 2: {
                unionFindSystem.run();
                break;
            }
            case 3: {
                huffmanSystem.run();
                break;
            }
            case 4: {
                decisionTreeSystem.run();
                break;
            }
            case 0: {
                cout << "\n感谢使用银行综合管理系统，再见！" << endl;
                break;
            }
            default: {
                cout << "\n无效选择，请重试！" << endl;
            }
        }
    } while (choice != 0);
    
    return 0;
}
