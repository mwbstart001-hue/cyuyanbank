#include "risk_module.h"

string creditLevelToString(CreditLevel level) {
    switch (level) {
        case CreditLevel::AAA: return "AAA";
        case CreditLevel::AA: return "AA";
        case CreditLevel::A: return "A";
        case CreditLevel::B: return "B";
        case CreditLevel::C: return "C";
        default: return "C";
    }
}

CreditLevel stringToCreditLevel(const string& str) {
    if (str == "AAA") return CreditLevel::AAA;
    if (str == "AA") return CreditLevel::AA;
    if (str == "A") return CreditLevel::A;
    if (str == "B") return CreditLevel::B;
    return CreditLevel::C;
}

string CustomerData::toString() const {
    ostringstream oss;
    oss << customerId << "|"
        << customerName << "|"
        << idCard << "|"
        << monthlyIncome << "|"
        << totalAssets << "|"
        << totalDebt << "|"
        << creditScore << "|"
        << loanCount << "|"
        << overdueCount << "|"
        << registerTime.toString() << "|"
        << creditLevelToString(creditLevel) << "|"
        << riskScore << "|"
        << (isHighRisk ? "1" : "0") << "|"
        << remark;
    return oss.str();
}

CustomerData CustomerData::fromString(const string& str) {
    CustomerData c;
    vector<string> parts = splitString(str, '|');
    if (parts.size() >= 14) {
        c.customerId = parts[0];
        c.customerName = parts[1];
        c.idCard = parts[2];
        c.monthlyIncome = stod(parts[3]);
        c.totalAssets = stod(parts[4]);
        c.totalDebt = stod(parts[5]);
        c.creditScore = stoi(parts[6]);
        c.loanCount = stoi(parts[7]);
        c.overdueCount = stoi(parts[8]);
        c.registerTime = Date::fromString(parts[9]);
        c.creditLevel = stringToCreditLevel(parts[10]);
        c.riskScore = stod(parts[11]);
        c.isHighRisk = (parts[12] == "1");
        c.remark = parts[13];
    }
    return c;
}

DecisionTree::DecisionTree() : root(nullptr) {
    buildDefaultTree();
}

DecisionTree::~DecisionTree() {
    deleteTree(root);
}

void DecisionTree::deleteTree(DecisionNode* node) {
    if (node) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

void DecisionTree::clear() {
    deleteTree(root);
    root = nullptr;
}

void DecisionTree::buildDefaultTree() {
    clear();
    
    root = new DecisionNode();
    root->attribute = "creditScore";
    root->threshold = 700;
    
    root->left = new DecisionNode();
    root->left->attribute = "debtRatio";
    root->left->threshold = 0.3;
    
    root->left->left = new DecisionNode();
    root->left->left->decision = "AAA";
    
    root->left->right = new DecisionNode();
    root->left->right->attribute = "creditScore";
    root->left->right->threshold = 750;
    
    root->left->right->left = new DecisionNode();
    root->left->right->left->decision = "AA";
    
    root->left->right->right = new DecisionNode();
    root->left->right->right->decision = "A";
    
    root->right = new DecisionNode();
    root->right->attribute = "creditScore";
    root->right->threshold = 600;
    
    root->right->left = new DecisionNode();
    root->right->left->attribute = "debtRatio";
    root->right->left->threshold = 0.5;
    
    root->right->left->left = new DecisionNode();
    root->right->left->left->decision = "A";
    
    root->right->left->right = new DecisionNode();
    root->right->left->right->decision = "B";
    
    root->right->right = new DecisionNode();
    root->right->right->attribute = "overdueCount";
    root->right->right->threshold = 3;
    
    root->right->right->left = new DecisionNode();
    root->right->right->left->decision = "B";
    
    root->right->right->right = new DecisionNode();
    root->right->right->right->decision = "C";
}

void DecisionTree::train() {
    buildDefaultTree();
}

CreditLevel DecisionTree::evaluate(CustomerData& customer, DecisionNode* node) {
    if (!node) return CreditLevel::C;
    
    if (node->isLeaf()) {
        return stringToCreditLevel(node->decision);
    }
    
    double value = 0;
    if (node->attribute == "creditScore") {
        value = customer.creditScore;
    } else if (node->attribute == "debtRatio") {
        if (customer.totalAssets > 0) {
            value = customer.totalDebt / customer.totalAssets;
        }
    } else if (node->attribute == "monthlyIncome") {
        value = customer.monthlyIncome;
    } else if (node->attribute == "overdueCount") {
        value = customer.overdueCount;
    } else if (node->attribute == "loanCount") {
        value = customer.loanCount;
    }
    
    if (value >= node->threshold) {
        return evaluate(customer, node->left);
    } else {
        return evaluate(customer, node->right);
    }
}

CreditLevel DecisionTree::predict(CustomerData& customer) {
    return evaluate(customer, root);
}

void DecisionTree::setRule(const string& attribute, double threshold) {
    if (root && !root->isLeaf()) {
        if (root->attribute == attribute) {
            root->threshold = threshold;
        }
    }
}

void DecisionTree::displayRules() {
    cout << "\n========== 决策树规则 ==========" << endl;
    cout << "根节点: " << root->attribute << " >= " << root->threshold << endl;
    
    if (root->left) {
        cout << "  左分支: " << root->left->attribute;
        if (!root->left->isLeaf()) {
            cout << " >= " << root->left->threshold;
        } else {
            cout << " -> " << root->left->decision;
        }
        cout << endl;
    }
    
    if (root->right) {
        cout << "  右分支: " << root->right->attribute;
        if (!root->right->isLeaf()) {
            cout << " >= " << root->right->threshold;
        } else {
            cout << " -> " << root->right->decision;
        }
        cout << endl;
    }
    
    cout << "\n评估维度:" << endl;
    cout << "  - 征信分数 (creditScore)" << endl;
    cout << "  - 负债率 (debtRatio = 总负债/总资产)" << endl;
    cout << "  - 月收入 (monthlyIncome)" << endl;
    cout << "  - 逾期次数 (overdueCount)" << endl;
    cout << "  - 贷款次数 (loanCount)" << endl;
}

RiskAssessment::RiskAssessment()
    : customerFile("data/customers.txt") {
    loadCustomers();
}

RiskAssessment::~RiskAssessment() {
    saveCustomers();
}

double RiskAssessment::calculateRiskScore(const CustomerData& customer) {
    double score = 0;
    
    score += customer.creditScore * 0.3;
    
    if (customer.totalAssets > 0) {
        double debtRatio = customer.totalDebt / customer.totalAssets;
        score += (1 - debtRatio) * 200 * 0.25;
    }
    
    score += min(customer.monthlyIncome / 100, 100.0) * 0.2;
    
    score -= customer.overdueCount * 20;
    
    score -= customer.loanCount * 2;
    
    return max(0.0, min(100.0, score));
}

bool RiskAssessment::checkHighRisk(const CustomerData& customer) {
    if (customer.creditScore < 600) return true;
    if (customer.overdueCount > 3) return true;
    if (customer.totalAssets > 0 && 
        customer.totalDebt / customer.totalAssets > 0.7) return true;
    if (customer.creditLevel == CreditLevel::C) return true;
    
    return false;
}

string RiskAssessment::addCustomer(const string& name, const string& idCard,
                                    double monthlyIncome, double totalAssets,
                                    double totalDebt, int creditScore,
                                    int loanCount, int overdueCount) {
    string customerId = generateId("CUS");
    
    CustomerData c;
    c.customerId = customerId;
    c.customerName = name;
    c.idCard = idCard;
    c.monthlyIncome = monthlyIncome;
    c.totalAssets = totalAssets;
    c.totalDebt = totalDebt;
    c.creditScore = creditScore;
    c.loanCount = loanCount;
    c.overdueCount = overdueCount;
    c.registerTime = Date::now();
    
    c.creditLevel = decisionTree.predict(c);
    c.riskScore = calculateRiskScore(c);
    c.isHighRisk = checkHighRisk(c);
    
    customers[customerId] = c;
    saveCustomers();
    
    return customerId;
}

bool RiskAssessment::updateCustomer(const string& customerId,
                                     double monthlyIncome,
                                     double totalAssets,
                                     double totalDebt,
                                     int creditScore,
                                     int loanCount,
                                     int overdueCount) {
    if (customers.find(customerId) == customers.end()) {
        return false;
    }
    
    CustomerData& c = customers[customerId];
    
    if (monthlyIncome >= 0) c.monthlyIncome = monthlyIncome;
    if (totalAssets >= 0) c.totalAssets = totalAssets;
    if (totalDebt >= 0) c.totalDebt = totalDebt;
    if (creditScore >= 0) c.creditScore = creditScore;
    if (loanCount >= 0) c.loanCount = loanCount;
    if (overdueCount >= 0) c.overdueCount = overdueCount;
    
    c.creditLevel = decisionTree.predict(c);
    c.riskScore = calculateRiskScore(c);
    c.isHighRisk = checkHighRisk(c);
    
    saveCustomers();
    return true;
}

bool RiskAssessment::deleteCustomer(const string& customerId) {
    if (customers.find(customerId) == customers.end()) {
        return false;
    }
    
    customers.erase(customerId);
    saveCustomers();
    return true;
}

CustomerData* RiskAssessment::getCustomer(const string& customerId) {
    if (customers.find(customerId) != customers.end()) {
        return &customers[customerId];
    }
    return nullptr;
}

vector<CustomerData> RiskAssessment::getAllCustomers(SortOrder order, const string& sortBy) {
    vector<CustomerData> result;
    for (const auto& pair : customers) {
        result.push_back(pair.second);
    }
    
    if (sortBy == "level") {
        sort(result.begin(), result.end(), 
             [order](const CustomerData& a, const CustomerData& b) -> bool {
                 if (order == SortOrder::ASC) {
                     return a.creditLevel < b.creditLevel;
                 }
                 return a.creditLevel > b.creditLevel;
             });
    } else if (sortBy == "score") {
        sort(result.begin(), result.end(), 
             [order](const CustomerData& a, const CustomerData& b) -> bool {
                 if (order == SortOrder::ASC) {
                     return a.riskScore < b.riskScore;
                 }
                 return a.riskScore > b.riskScore;
             });
    } else {
        sort(result.begin(), result.end(), 
             [order](const CustomerData& a, const CustomerData& b) -> bool {
                 if (order == SortOrder::ASC) {
                     return a.registerTime < b.registerTime;
                 }
                 return a.registerTime > b.registerTime;
             });
    }
    
    return result;
}

vector<CustomerData> RiskAssessment::getHighRiskCustomers() {
    vector<CustomerData> result;
    for (const auto& pair : customers) {
        if (pair.second.isHighRisk) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<CustomerData> RiskAssessment::getByCreditLevel(CreditLevel level) {
    vector<CustomerData> result;
    for (const auto& pair : customers) {
        if (pair.second.creditLevel == level) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void RiskAssessment::assessCustomer(const string& customerId) {
    if (customers.find(customerId) == customers.end()) {
        cout << "客户不存在！" << endl;
        return;
    }
    
    CustomerData& c = customers[customerId];
    c.creditLevel = decisionTree.predict(c);
    c.riskScore = calculateRiskScore(c);
    c.isHighRisk = checkHighRisk(c);
    
    saveCustomers();
    
    cout << "\n=== 评估完成 ===" << endl;
    cout << "客户: " << c.customerName << endl;
    cout << "信用等级: " << creditLevelToString(c.creditLevel) << endl;
    cout << "风险分数: " << fixed << setprecision(2) << c.riskScore << endl;
    cout << "高风险标记: " << (c.isHighRisk ? "是" : "否") << endl;
}

void RiskAssessment::assessAllCustomers() {
    for (auto& pair : customers) {
        pair.second.creditLevel = decisionTree.predict(pair.second);
        pair.second.riskScore = calculateRiskScore(pair.second);
        pair.second.isHighRisk = checkHighRisk(pair.second);
    }
    saveCustomers();
    cout << "所有客户已重新评估！" << endl;
}

void RiskAssessment::adjustRules() {
    cout << "\n========== 调整评估规则 ==========" << endl;
    cout << "当前规则:" << endl;
    displayRules();
    
    cout << "\n请选择要调整的规则:" << endl;
    cout << "1. 征信分数阈值" << endl;
    cout << "2. 负债率阈值" << endl;
    cout << "3. 重置为默认规则" << endl;
    cout << "0. 返回" << endl;
    cout << "请选择: ";
    
    int choice;
    cin >> choice;
    
    switch (choice) {
        case 1: {
            cout << "输入新的征信分数阈值 (当前: 700): ";
            double threshold;
            cin >> threshold;
            decisionTree.setRule("creditScore", threshold);
            cout << "规则已更新！" << endl;
            break;
        }
        case 2: {
            cout << "输入新的负债率阈值 (当前: 0.3): ";
            double threshold;
            cin >> threshold;
            decisionTree.setRule("debtRatio", threshold);
            cout << "规则已更新！" << endl;
            break;
        }
        case 3:
            decisionTree.train();
            cout << "已重置为默认规则！" << endl;
            break;
        default:
            break;
    }
}

void RiskAssessment::displayRules() {
    decisionTree.displayRules();
}

bool RiskAssessment::loadCustomers() {
    ifstream file(customerFile);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        CustomerData c = CustomerData::fromString(line);
        customers[c.customerId] = c;
    }
    
    file.close();
    return true;
}

bool RiskAssessment::saveCustomers() {
    ofstream file(customerFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& pair : customers) {
        file << pair.second.toString() << "\n";
    }
    
    file.close();
    return true;
}

void RiskAssessment::displayCustomer(const string& customerId) {
    CustomerData* c = getCustomer(customerId);
    if (!c) {
        cout << "客户不存在！" << endl;
        return;
    }
    
    cout << "\n========== 客户详情 ==========" << endl;
    cout << "客户ID: " << c->customerId << endl;
    cout << "姓名: " << c->customerName << endl;
    cout << "身份证: " << c->idCard << endl;
    cout << "月收入: " << fixed << setprecision(2) << c->monthlyIncome << endl;
    cout << "总资产: " << c->totalAssets << endl;
    cout << "总负债: " << c->totalDebt << endl;
    cout << "征信分数: " << c->creditScore << endl;
    cout << "贷款次数: " << c->loanCount << endl;
    cout << "逾期次数: " << c->overdueCount << endl;
    cout << "注册时间: " << c->registerTime.toString() << endl;
    cout << "信用等级: " << creditLevelToString(c->creditLevel) << endl;
    cout << "风险分数: " << c->riskScore << endl;
    cout << "高风险标记: " << (c->isHighRisk ? "是" : "否") << endl;
    cout << "备注: " << c->remark << endl;
}

void RiskAssessment::displayAllCustomers() {
    cout << "\n========== 所有客户列表 ==========" << endl;
    
    vector<CustomerData> allCustomers = getAllCustomers();
    if (allCustomers.empty()) {
        cout << "暂无客户" << endl;
        return;
    }
    
    cout << left << setw(12) << "客户ID"
         << setw(10) << "姓名"
         << setw(10) << "信用等级"
         << setw(12) << "风险分数"
         << setw(10) << "高风险"
         << setw(20) << "注册时间" << endl;
    cout << string(74, '-') << endl;
    
    for (const auto& c : allCustomers) {
        cout << left << setw(12) << c.customerId
             << setw(10) << c.customerName
             << setw(10) << creditLevelToString(c.creditLevel)
             << setw(12) << fixed << setprecision(2) << c.riskScore
             << setw(10) << (c.isHighRisk ? "是" : "否")
             << setw(20) << c.registerTime.toString() << endl;
    }
}

void RiskAssessment::displayStatistics() {
    cout << "\n========== 风险评估统计 ==========" << endl;
    
    int aaa = 0, aa = 0, a = 0, b = 0, c = 0;
    int highRisk = 0;
    double avgScore = 0;
    
    for (const auto& pair : customers) {
        switch (pair.second.creditLevel) {
            case CreditLevel::AAA: aaa++; break;
            case CreditLevel::AA: aa++; break;
            case CreditLevel::A: a++; break;
            case CreditLevel::B: b++; break;
            case CreditLevel::C: c++; break;
        }
        if (pair.second.isHighRisk) highRisk++;
        avgScore += pair.second.riskScore;
    }
    
    int total = customers.size();
    if (total > 0) avgScore /= total;
    
    cout << "总客户数: " << total << endl;
    cout << "\n信用等级分布:" << endl;
    cout << "  AAA: " << aaa << endl;
    cout << "  AA: " << aa << endl;
    cout << "  A: " << a << endl;
    cout << "  B: " << b << endl;
    cout << "  C: " << c << endl;
    cout << "\n高风险客户: " << highRisk << endl;
    cout << "平均风险分数: " << fixed << setprecision(2) << avgScore << endl;
}

void RiskAssessment::displayHighRiskCustomers() {
    cout << "\n========== 高风险客户列表 ==========" << endl;
    
    vector<CustomerData> highRiskList = getHighRiskCustomers();
    if (highRiskList.empty()) {
        cout << "暂无高风险客户" << endl;
        return;
    }
    
    cout << left << setw(12) << "客户ID"
         << setw(10) << "姓名"
         << setw(10) << "信用等级"
         << setw(10) << "征信分数"
         << setw(10) << "逾期次数" << endl;
    cout << string(52, '-') << endl;
    
    for (const auto& c : highRiskList) {
        cout << left << setw(12) << c.customerId
             << setw(10) << c.customerName
             << setw(10) << creditLevelToString(c.creditLevel)
             << setw(10) << c.creditScore
             << setw(10) << c.overdueCount << endl;
    }
}
