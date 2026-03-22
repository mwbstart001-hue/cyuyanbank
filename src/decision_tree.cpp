#include "../include/decision_tree.h"
#include <cstring>

DecisionTreeSystem::DecisionTreeSystem(int cap) {
    customerCapacity = cap;
    customers = new CustomerData[customerCapacity];
    customerCount = 0;
    
    evalCapacity = cap;
    evaluations = new EvaluationResult[evalCapacity];
    evalCount = 0;
    
    root = nullptr;
    
    // 默认规则
    rules.incomeThreshold = 50000;
    rules.creditScoreThreshold = 700;
    rules.debtRatioThreshold = 0.5;
    rules.employmentYearsThreshold = 2;
    
    root = buildTree();
}

DecisionTreeSystem::~DecisionTreeSystem() {
    delete[] customers;
    delete[] evaluations;
    deleteTree(root);
}

void DecisionTreeSystem::deleteTree(DecisionNode* node) {
    if (node == nullptr) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

DecisionNode* DecisionTreeSystem::buildTree() {
    // 构建决策树
    // 根节点：收入判断
    DecisionNode* root = new DecisionNode();
    root->condition = "年收入 >= " + to_string((int)rules.incomeThreshold);
    root->attributeIndex = 0;
    root->threshold = rules.incomeThreshold;
    root->isLeaf = false;
    
    // 左子树：高收入分支
    // 信用分判断
    DecisionNode* creditNode = new DecisionNode();
    creditNode->condition = "信用分 >= " + to_string(rules.creditScoreThreshold);
    creditNode->attributeIndex = 1;
    creditNode->threshold = rules.creditScoreThreshold;
    creditNode->isLeaf = false;
    
    // 高信用分 -> AAA
    DecisionNode* aaaNode = new DecisionNode();
    aaaNode->isLeaf = true;
    aaaNode->result = AAA;
    creditNode->left = aaaNode;
    
    // 低信用分 -> AA
    DecisionNode* aaNode = new DecisionNode();
    aaNode->isLeaf = true;
    aaNode->result = AA;
    creditNode->right = aaNode;
    
    root->left = creditNode;
    
    // 右子树：低收入分支
    // 负债率判断
    DecisionNode* debtNode = new DecisionNode();
    debtNode->condition = "负债率 < " + to_string((int)(rules.debtRatioThreshold * 100)) + "%";
    debtNode->attributeIndex = 2;
    debtNode->threshold = rules.debtRatioThreshold;
    debtNode->isLeaf = false;
    
    // 低负债率 -> A
    DecisionNode* aNode = new DecisionNode();
    aNode->isLeaf = true;
    aNode->result = A;
    debtNode->left = aNode;
    
    // 高负债率 -> 工作年限判断
    DecisionNode* empNode = new DecisionNode();
    empNode->condition = "工作年限 >= " + to_string(rules.employmentYearsThreshold) + "年";
    empNode->attributeIndex = 3;
    empNode->threshold = rules.employmentYearsThreshold;
    empNode->isLeaf = false;
    
    // 工作年限长 -> B
    DecisionNode* bNode = new DecisionNode();
    bNode->isLeaf = true;
    bNode->result = B;
    empNode->left = bNode;
    
    // 工作年限短 -> C
    DecisionNode* cNode = new DecisionNode();
    cNode->isLeaf = true;
    cNode->result = C;
    empNode->right = cNode;
    
    debtNode->right = empNode;
    root->right = debtNode;
    
    return root;
}

CreditLevel DecisionTreeSystem::evaluateNode(DecisionNode* node, const CustomerData& data) {
    if (node->isLeaf) {
        return node->result;
    }
    
    double value = 0;
    switch (node->attributeIndex) {
        case 0: value = data.income; break;
        case 1: value = data.creditScore; break;
        case 2: value = data.debtRatio; break;
        case 3: value = data.employmentYears; break;
    }
    
    if (value >= node->threshold) {
        return evaluateNode(node->left, data);
    } else {
        return evaluateNode(node->right, data);
    }
}

int DecisionTreeSystem::calculateRiskScore(const CustomerData& data) {
    int score = 50; // 基础分
    
    // 收入评分 (0-25分)
    if (data.income >= 100000) score += 25;
    else if (data.income >= 50000) score += 20;
    else if (data.income >= 30000) score += 15;
    else if (data.income >= 10000) score += 10;
    else score += 5;
    
    // 信用分评分 (0-25分)
    if (data.creditScore >= 800) score += 25;
    else if (data.creditScore >= 700) score += 20;
    else if (data.creditScore >= 600) score += 15;
    else if (data.creditScore >= 500) score += 10;
    else score += 5;
    
    // 负债率评分 (0-25分，负债率越低分越高)
    if (data.debtRatio <= 0.2) score += 25;
    else if (data.debtRatio <= 0.4) score += 20;
    else if (data.debtRatio <= 0.6) score += 15;
    else if (data.debtRatio <= 0.8) score += 10;
    else score += 5;
    
    // 工作年限评分 (0-15分)
    if (data.employmentYears >= 10) score += 15;
    else if (data.employmentYears >= 5) score += 12;
    else if (data.employmentYears >= 2) score += 8;
    else score += 4;
    
    // 违约记录扣分
    if (data.hasDefaultHistory) score -= 20;
    
    return max(0, min(100, score));
}

bool DecisionTreeSystem::addCustomer(int id, const string& name, double income, 
                                      int creditScore, double debtRatio, 
                                      int employmentYears, bool hasDefault) {
    if (customerCount >= customerCapacity) {
        cout << "客户数量已满！" << endl;
        return false;
    }
    
    customers[customerCount].id = id;
    customers[customerCount].name = name;
    customers[customerCount].income = income;
    customers[customerCount].creditScore = creditScore;
    customers[customerCount].debtRatio = debtRatio;
    customers[customerCount].employmentYears = employmentYears;
    customers[customerCount].hasDefaultHistory = hasDefault;
    customerCount++;
    
    cout << "客户添加成功！ID: " << id << ", 姓名: " << name << endl;
    return true;
}

bool DecisionTreeSystem::updateCustomer(int id, const string& field, double value) {
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id == id) {
            if (field == "income") {
                customers[i].income = value;
            } else if (field == "creditScore") {
                customers[i].creditScore = (int)value;
            } else if (field == "debtRatio") {
                customers[i].debtRatio = value;
            } else if (field == "employmentYears") {
                customers[i].employmentYears = (int)value;
            } else {
                cout << "未知字段: " << field << endl;
                return false;
            }
            cout << "客户 " << id << " 信息更新成功！" << endl;
            return true;
        }
    }
    cout << "未找到客户 " << id << endl;
    return false;
}

CustomerData* DecisionTreeSystem::getCustomer(int id) {
    for (int i = 0; i < customerCount; i++) {
        if (customers[i].id == id) {
            return &customers[i];
        }
    }
    return nullptr;
}

void DecisionTreeSystem::showAllCustomers() {
    cout << "\n========== 所有客户 ==========" << endl;
    cout << "总客户数: " << customerCount << endl;
    
    for (int i = 0; i < customerCount; i++) {
        cout << "  ID: " << customers[i].id
             << ", 姓名: " << customers[i].name
             << ", 年收入: " << customers[i].income
             << ", 信用分: " << customers[i].creditScore
             << ", 负债率: " << (customers[i].debtRatio * 100) << "%"
             << ", 工作年限: " << customers[i].employmentYears << "年"
             << ", 违约记录: " << (customers[i].hasDefaultHistory ? "有" : "无") << endl;
    }
    cout << "==============================\n" << endl;
}

EvaluationResult DecisionTreeSystem::evaluateCustomer(int customerId) {
    EvaluationResult result;
    result.customerId = customerId;
    result.evaluationTime = Utils::getCurrentTime();
    
    CustomerData* customer = getCustomer(customerId);
    if (customer == nullptr) {
        result.level = C;
        result.riskScore = 0;
        result.isHighRisk = true;
        result.remarks = "客户不存在";
        return result;
    }
    
    // 使用决策树评估
    result.level = evaluateNode(root, *customer);
    
    // 计算风险分
    result.riskScore = calculateRiskScore(*customer);
    
    // 高风险判断 (风险分 < 40 或有违约记录)
    result.isHighRisk = (result.riskScore < 40) || customer->hasDefaultHistory;
    
    // 保存评估结果
    if (evalCount < evalCapacity) {
        evaluations[evalCount] = result;
        evalCount++;
    }
    
    return result;
}

void DecisionTreeSystem::evaluateAll() {
    cout << "\n========== 批量评估 ==========" << endl;
    
    for (int i = 0; i < customerCount; i++) {
        EvaluationResult result = evaluateCustomer(customers[i].id);
        
        string levelStr;
        switch (result.level) {
            case AAA: levelStr = "AAA"; break;
            case AA: levelStr = "AA"; break;
            case A: levelStr = "A"; break;
            case B: levelStr = "B"; break;
            case C: levelStr = "C"; break;
        }
        
        cout << "  客户 " << customers[i].name << " (ID: " << customers[i].id << "): "
             << "等级 " << levelStr << ", 风险分 " << result.riskScore;
        if (result.isHighRisk) {
            cout << " [高风险]";
        }
        cout << endl;
    }
    
    cout << "==============================\n" << endl;
}

void DecisionTreeSystem::showEvaluation(int customerId) {
    for (int i = 0; i < evalCount; i++) {
        if (evaluations[i].customerId == customerId) {
            string levelStr;
            switch (evaluations[i].level) {
                case AAA: levelStr = "AAA"; break;
                case AA: levelStr = "AA"; break;
                case A: levelStr = "A"; break;
                case B: levelStr = "B"; break;
                case C: levelStr = "C"; break;
            }
            
            cout << "\n========== 评估结果 ==========" << endl;
            cout << "客户ID: " << evaluations[i].customerId << endl;
            cout << "信用等级: " << levelStr << endl;
            cout << "风险分值: " << evaluations[i].riskScore << endl;
            cout << "评估时间: " << evaluations[i].evaluationTime << endl;
            cout << "高风险标记: " << (evaluations[i].isHighRisk ? "是" : "否") << endl;
            if (!evaluations[i].remarks.empty()) {
                cout << "备注: " << evaluations[i].remarks << endl;
            }
            cout << "==============================\n" << endl;
            return;
        }
    }
    cout << "未找到客户 " << customerId << " 的评估记录" << endl;
}

void DecisionTreeSystem::showAllEvaluations() {
    cout << "\n========== 所有评估结果 ==========" << endl;
    cout << "总评估数: " << evalCount << endl;
    
    for (int i = 0; i < evalCount; i++) {
        string levelStr;
        switch (evaluations[i].level) {
            case AAA: levelStr = "AAA"; break;
            case AA: levelStr = "AA"; break;
            case A: levelStr = "A"; break;
            case B: levelStr = "B"; break;
            case C: levelStr = "C"; break;
        }
        
        cout << "  客户ID: " << evaluations[i].customerId
             << ", 等级: " << levelStr
             << ", 风险分: " << evaluations[i].riskScore
             << ", 高风险: " << (evaluations[i].isHighRisk ? "是" : "否") << endl;
    }
    cout << "==================================\n" << endl;
}

void DecisionTreeSystem::adjustRules(double income, int credit, double debt, int employment) {
    rules.incomeThreshold = income;
    rules.creditScoreThreshold = credit;
    rules.debtRatioThreshold = debt;
    rules.employmentYearsThreshold = employment;
    
    // 重建决策树
    deleteTree(root);
    root = buildTree();
    
    cout << "评估规则已更新！" << endl;
    showCurrentRules();
}

void DecisionTreeSystem::showCurrentRules() {
    cout << "\n========== 当前评估规则 ==========" << endl;
    cout << "收入阈值: " << rules.incomeThreshold << endl;
    cout << "信用分阈值: " << rules.creditScoreThreshold << endl;
    cout << "负债率阈值: " << (rules.debtRatioThreshold * 100) << "%" << endl;
    cout << "工作年限阈值: " << rules.employmentYearsThreshold << "年" << endl;
    cout << "==================================\n" << endl;
}

void DecisionTreeSystem::sortByCreditLevel() {
    cout << "\n========== 按信用等级排序 ==========" << endl;
    
    // 冒泡排序
    for (int i = 0; i < evalCount - 1; i++) {
        for (int j = 0; j < evalCount - i - 1; j++) {
            if (evaluations[j].level < evaluations[j + 1].level) {
                EvaluationResult temp = evaluations[j];
                evaluations[j] = evaluations[j + 1];
                evaluations[j + 1] = temp;
            }
        }
    }
    
    showAllEvaluations();
}

void DecisionTreeSystem::sortByRiskScore() {
    cout << "\n========== 按风险分值排序 ==========" << endl;
    
    // 冒泡排序
    for (int i = 0; i < evalCount - 1; i++) {
        for (int j = 0; j < evalCount - i - 1; j++) {
            if (evaluations[j].riskScore > evaluations[j + 1].riskScore) {
                EvaluationResult temp = evaluations[j];
                evaluations[j] = evaluations[j + 1];
                evaluations[j + 1] = temp;
            }
        }
    }
    
    showAllEvaluations();
}

void DecisionTreeSystem::showHighRiskCustomers() {
    cout << "\n========== 高风险客户列表 ==========" << endl;
    
    bool found = false;
    for (int i = 0; i < evalCount; i++) {
        if (evaluations[i].isHighRisk) {
            string levelStr;
            switch (evaluations[i].level) {
                case AAA: levelStr = "AAA"; break;
                case AA: levelStr = "AA"; break;
                case A: levelStr = "A"; break;
                case B: levelStr = "B"; break;
                case C: levelStr = "C"; break;
            }
            
            cout << "  客户ID: " << evaluations[i].customerId
                 << ", 等级: " << levelStr
                 << ", 风险分: " << evaluations[i].riskScore << endl;
            found = true;
        }
    }
    
    if (!found) {
        cout << "  暂无高风险客户" << endl;
    }
    cout << "====================================\n" << endl;
}

bool DecisionTreeSystem::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << customerCount << endl;
    for (int i = 0; i < customerCount; i++) {
        file << customers[i].id << " "
             << customers[i].name << " "
             << customers[i].income << " "
             << customers[i].creditScore << " "
             << customers[i].debtRatio << " "
             << customers[i].employmentYears << " "
             << customers[i].hasDefaultHistory << endl;
    }
    
    file.close();
    return true;
}

bool DecisionTreeSystem::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> customerCount;
    for (int i = 0; i < customerCount; i++) {
        file >> customers[i].id
             >> customers[i].name
             >> customers[i].income
             >> customers[i].creditScore
             >> customers[i].debtRatio
             >> customers[i].employmentYears
             >> customers[i].hasDefaultHistory;
    }
    
    file.close();
    return true;
}

bool DecisionTreeSystem::saveEvaluations(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << evalCount << endl;
    for (int i = 0; i < evalCount; i++) {
        file << evaluations[i].customerId << " "
             << evaluations[i].level << " "
             << evaluations[i].riskScore << " "
             << evaluations[i].evaluationTime << " "
             << evaluations[i].isHighRisk << " "
             << evaluations[i].remarks << endl;
    }
    
    file.close();
    return true;
}

bool DecisionTreeSystem::loadEvaluations(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> evalCount;
    for (int i = 0; i < evalCount; i++) {
        int level;
        file >> evaluations[i].customerId
             >> level
             >> evaluations[i].riskScore
             >> evaluations[i].evaluationTime
             >> evaluations[i].isHighRisk;
        evaluations[i].level = static_cast<CreditLevel>(level);
        file.ignore();
        getline(file, evaluations[i].remarks);
    }
    
    file.close();
    return true;
}

void DecisionTreeSystem::showMenu() {
    cout << "\n========== 风险评估系统 ==========" << endl;
    cout << "1. 录入客户数据" << endl;
    cout << "2. 更新客户数据" << endl;
    cout << "3. 评估单个客户" << endl;
    cout << "4. 批量评估所有客户" << endl;
    cout << "5. 查看评估结果" << endl;
    cout << "6. 查看所有评估" << endl;
    cout << "7. 调整评估规则" << endl;
    cout << "8. 查看当前规则" << endl;
    cout << "9. 按信用等级排序" << endl;
    cout << "10. 按风险分值排序" << endl;
    cout << "11. 查看高风险客户" << endl;
    cout << "12. 查看所有客户" << endl;
    cout << "13. 保存数据" << endl;
    cout << "14. 加载数据" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "==================================" << endl;
    cout << "请选择: ";
}

void DecisionTreeSystem::run() {
    int choice;
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                int id, creditScore, employmentYears;
                string name;
                double income, debtRatio;
                bool hasDefault;
                cout << "请输入客户ID: ";
                cin >> id;
                cout << "请输入姓名: ";
                cin >> name;
                cout << "请输入年收入: ";
                cin >> income;
                cout << "请输入信用分(0-850): ";
                cin >> creditScore;
                cout << "请输入负债率(0-1): ";
                cin >> debtRatio;
                cout << "请输入工作年限: ";
                cin >> employmentYears;
                cout << "是否有违约记录(0/1): ";
                cin >> hasDefault;
                addCustomer(id, name, income, creditScore, debtRatio, employmentYears, hasDefault);
                break;
            }
            case 2: {
                int id;
                string field;
                double value;
                cout << "请输入客户ID: ";
                cin >> id;
                cout << "请输入要更新的字段(income/creditScore/debtRatio/employmentYears): ";
                cin >> field;
                cout << "请输入新值: ";
                cin >> value;
                updateCustomer(id, field, value);
                break;
            }
            case 3: {
                int customerId;
                cout << "请输入客户ID: ";
                cin >> customerId;
                EvaluationResult result = evaluateCustomer(customerId);
                showEvaluation(customerId);
                break;
            }
            case 4: {
                evaluateAll();
                break;
            }
            case 5: {
                int customerId;
                cout << "请输入客户ID: ";
                cin >> customerId;
                showEvaluation(customerId);
                break;
            }
            case 6: {
                showAllEvaluations();
                break;
            }
            case 7: {
                double income, debt;
                int credit, employment;
                cout << "请输入收入阈值: ";
                cin >> income;
                cout << "请输入信用分阈值: ";
                cin >> credit;
                cout << "请输入负债率阈值(0-1): ";
                cin >> debt;
                cout << "请输入工作年限阈值: ";
                cin >> employment;
                adjustRules(income, credit, debt, employment);
                break;
            }
            case 8: {
                showCurrentRules();
                break;
            }
            case 9: {
                sortByCreditLevel();
                break;
            }
            case 10: {
                sortByRiskScore();
                break;
            }
            case 11: {
                showHighRiskCustomers();
                break;
            }
            case 12: {
                showAllCustomers();
                break;
            }
            case 13: {
                if (saveToFile("customer_data.txt") && saveEvaluations("evaluation_data.txt")) {
                    cout << "数据保存成功！" << endl;
                } else {
                    cout << "数据保存失败！" << endl;
                }
                break;
            }
            case 14: {
                if (loadFromFile("customer_data.txt") && loadEvaluations("evaluation_data.txt")) {
                    cout << "数据加载成功！" << endl;
                } else {
                    cout << "数据加载失败！" << endl;
                }
                break;
            }
            case 0: {
                cout << "返回主菜单..." << endl;
                break;
            }
            default: {
                cout << "无效选择，请重试！" << endl;
            }
        }
    } while (choice != 0);
}
