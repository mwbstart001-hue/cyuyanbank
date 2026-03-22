#include "risk_assessment.h"

// DecisionTree implementation
DecisionTree::DecisionTree() : root(nullptr) {}

DecisionTree::~DecisionTree() {
    destroyTree(root);
}

void DecisionTree::destroyTree(DecisionTreeNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

int DecisionTree::evaluateNode(DecisionTreeNode* node, const CustomerData& data) const {
    if (!node) return 0;

    if (node->isLeaf) {
        return node->score;
    }

    double value = 0.0;
    if (node->attribute == "creditScore") {
        value = data.creditScore;
    } else if (node->attribute == "debtRatio") {
        value = data.debtRatio;
    } else if (node->attribute == "monthlyIncome") {
        value = data.monthlyIncome;
    } else if (node->attribute == "age") {
        value = data.age;
    }

    if (value <= node->threshold) {
        return node->score + evaluateNode(node->left, data);
    } else {
        return node->score + evaluateNode(node->right, data);
    }
}

void DecisionTree::buildTree(const vector<AssessmentRule>& assessmentRules) {
    destroyTree(root);
    rules = assessmentRules;

    root = new DecisionTreeNode();
    root->attribute = "creditScore";
    root->threshold = 600.0;

    DecisionTreeNode* creditLow = new DecisionTreeNode();
    creditLow->attribute = "debtRatio";
    creditLow->threshold = 0.5;
    creditLow->score = 10;

    DecisionTreeNode* creditHigh = new DecisionTreeNode();
    creditHigh->attribute = "debtRatio";
    creditHigh->threshold = 0.3;
    creditHigh->score = 30;

    root->left = creditLow;
    root->right = creditHigh;

    DecisionTreeNode* leaf1 = new DecisionTreeNode();
    leaf1->isLeaf = true;
    leaf1->score = 5;
    leaf1->rating = C;

    DecisionTreeNode* leaf2 = new DecisionTreeNode();
    leaf2->isLeaf = true;
    leaf2->score = 15;
    leaf2->rating = B;

    creditLow->left = leaf1;
    creditLow->right = leaf2;

    DecisionTreeNode* leaf3 = new DecisionTreeNode();
    leaf3->isLeaf = true;
    leaf3->score = 25;
    leaf3->rating = A;

    DecisionTreeNode* leaf4 = new DecisionTreeNode();
    leaf4->isLeaf = true;
    leaf4->score = 40;
    leaf4->rating = AA;

    creditHigh->left = leaf3;
    creditHigh->right = leaf4;
}

int DecisionTree::evaluate(const CustomerData& data) const {
    return evaluateNode(root, data);
}

CreditRating DecisionTree::getRating(int totalScore) const {
    if (totalScore >= 85) return AAA;
    if (totalScore >= 70) return AA;
    if (totalScore >= 55) return A;
    if (totalScore >= 40) return B;
    return C;
}

void DecisionTree::updateRules(const vector<AssessmentRule>& newRules) {
    rules = newRules;
    buildTree(rules);
}

vector<AssessmentRule> DecisionTree::getRules() const {
    return rules;
}

// RiskAssessmentModule implementation
RiskAssessmentModule::RiskAssessmentModule() {
    initializeDefaultRules();
    decisionTree.buildTree(assessmentRules);
}

void RiskAssessmentModule::initializeDefaultRules() {
    AssessmentRule rule1;
    rule1.ruleId = "R001";
    rule1.dimension = "creditScore";
    rule1.minValue = 750;
    rule1.maxValue = 850;
    rule1.score = 30;
    rule1.description = "Excellent credit score";
    assessmentRules.push_back(rule1);

    AssessmentRule rule2;
    rule2.ruleId = "R002";
    rule2.dimension = "creditScore";
    rule2.minValue = 650;
    rule2.maxValue = 749;
    rule2.score = 20;
    rule2.description = "Good credit score";
    assessmentRules.push_back(rule2);

    AssessmentRule rule3;
    rule3.ruleId = "R003";
    rule3.dimension = "debtRatio";
    rule3.minValue = 0;
    rule3.maxValue = 0.2;
    rule3.score = 25;
    rule3.description = "Low debt ratio";
    assessmentRules.push_back(rule3);

    AssessmentRule rule4;
    rule4.ruleId = "R004";
    rule4.dimension = "debtRatio";
    rule4.minValue = 0.21;
    rule4.maxValue = 0.4;
    rule4.score = 15;
    rule4.description = "Moderate debt ratio";
    assessmentRules.push_back(rule4);

    AssessmentRule rule5;
    rule5.ruleId = "R005";
    rule5.dimension = "monthlyIncome";
    rule5.minValue = 50000;
    rule5.maxValue = 1000000;
    rule5.score = 25;
    rule5.description = "High income";
    assessmentRules.push_back(rule5);

    AssessmentRule rule6;
    rule6.ruleId = "R006";
    rule6.dimension = "monthlyIncome";
    rule6.minValue = 20000;
    rule6.maxValue = 49999;
    rule6.score = 15;
    rule6.description = "Medium income";
    assessmentRules.push_back(rule6);
}

int RiskAssessmentModule::calculateRiskScore(const CustomerData& data) const {
    int score = 0;

    if (data.creditScore >= 750) score += 30;
    else if (data.creditScore >= 650) score += 20;
    else if (data.creditScore >= 550) score += 10;
    else score += 5;

    if (data.debtRatio <= 0.2) score += 25;
    else if (data.debtRatio <= 0.4) score += 15;
    else if (data.debtRatio <= 0.6) score += 8;
    else score += 3;

    if (data.monthlyIncome >= 50000) score += 25;
    else if (data.monthlyIncome >= 20000) score += 15;
    else if (data.monthlyIncome >= 10000) score += 8;
    else score += 3;

    if (data.age >= 25 && data.age <= 55) score += 10;
    else if (data.age >= 22 && data.age <= 60) score += 5;
    else score += 2;

    if (data.employmentStatus == "Employed") score += 10;
    else if (data.employmentStatus == "Self-employed") score += 7;
    else score += 3;

    return score;
}

CreditRating RiskAssessmentModule::determineRating(int riskScore) const {
    if (riskScore >= 85) return AAA;
    if (riskScore >= 70) return AA;
    if (riskScore >= 55) return A;
    if (riskScore >= 40) return B;
    return C;
}

bool RiskAssessmentModule::isHighRisk(CreditRating rating) const {
    return rating == C || rating == B;
}

bool RiskAssessmentModule::addCustomerData(const CustomerData& data) {
    if (customerDatabase.find(data.customerId) != customerDatabase.end()) {
        return false;
    }
    customerDatabase[data.customerId] = data;
    return true;
}

bool RiskAssessmentModule::updateCustomerData(const string& customerId, const CustomerData& newData) {
    auto it = customerDatabase.find(customerId);
    if (it == customerDatabase.end()) {
        return false;
    }
    customerDatabase[customerId] = newData;
    return true;
}

CustomerData* RiskAssessmentModule::getCustomerData(const string& customerId) {
    auto it = customerDatabase.find(customerId);
    if (it != customerDatabase.end()) {
        return &it->second;
    }
    return nullptr;
}

const CustomerData* RiskAssessmentModule::getCustomerData(const string& customerId) const {
    auto it = customerDatabase.find(customerId);
    if (it != customerDatabase.end()) {
        return &it->second;
    }
    return nullptr;
}

vector<CustomerData> RiskAssessmentModule::getAllCustomers() const {
    vector<CustomerData> result;
    for (const auto& pair : customerDatabase) {
        result.push_back(pair.second);
    }
    return result;
}

bool RiskAssessmentModule::assessCustomer(const string& customerId, const string& assessor) {
    auto it = customerDatabase.find(customerId);
    if (it == customerDatabase.end()) {
        return false;
    }

    CustomerData& data = it->second;
    data.riskScore = calculateRiskScore(data);
    data.rating = determineRating(data.riskScore);
    data.isHighRisk = isHighRisk(data.rating);
    data.assessmentTime = time(nullptr);
    data.assessor = assessor;

    return true;
}

void RiskAssessmentModule::batchAssessment() {
    for (auto& pair : customerDatabase) {
        assessCustomer(pair.first);
    }
}

void RiskAssessmentModule::updateAssessmentRules(const vector<AssessmentRule>& rules) {
    assessmentRules = rules;
    decisionTree.updateRules(rules);
}

vector<AssessmentRule> RiskAssessmentModule::getAssessmentRules() const {
    return assessmentRules;
}

bool RiskAssessmentModule::addRule(const AssessmentRule& rule) {
    for (const auto& r : assessmentRules) {
        if (r.ruleId == rule.ruleId) {
            return false;
        }
    }
    assessmentRules.push_back(rule);
    decisionTree.updateRules(assessmentRules);
    return true;
}

bool RiskAssessmentModule::removeRule(const string& ruleId) {
    for (auto it = assessmentRules.begin(); it != assessmentRules.end(); ++it) {
        if (it->ruleId == ruleId) {
            assessmentRules.erase(it);
            decisionTree.updateRules(assessmentRules);
            return true;
        }
    }
    return false;
}

bool RiskAssessmentModule::enableRule(const string& ruleId, bool enable) {
    for (auto& rule : assessmentRules) {
        if (rule.ruleId == ruleId) {
            rule.isEnabled = enable;
            decisionTree.updateRules(assessmentRules);
            return true;
        }
    }
    return false;
}

vector<CustomerData> RiskAssessmentModule::getSortedByCreditRating(bool ascending) const {
    vector<CustomerData> result = getAllCustomers();
    sort(result.begin(), result.end(), [ascending](const CustomerData& a, const CustomerData& b) {
        return ascending ? (a.rating < b.rating) : (a.rating > b.rating);
    });
    return result;
}

vector<CustomerData> RiskAssessmentModule::getSortedByRiskScore(bool ascending) const {
    vector<CustomerData> result = getAllCustomers();
    sort(result.begin(), result.end(), [ascending](const CustomerData& a, const CustomerData& b) {
        return ascending ? (a.riskScore < b.riskScore) : (a.riskScore > b.riskScore);
    });
    return result;
}

vector<CustomerData> RiskAssessmentModule::getHighRiskCustomers() const {
    vector<CustomerData> result;
    for (const auto& pair : customerDatabase) {
        if (pair.second.isHighRisk) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<CustomerData> RiskAssessmentModule::queryByRating(CreditRating rating) const {
    vector<CustomerData> result;
    for (const auto& pair : customerDatabase) {
        if (pair.second.rating == rating) {
            result.push_back(pair.second);
        }
    }
    return result;
}

vector<CustomerData> RiskAssessmentModule::queryByRiskScoreRange(int minScore, int maxScore) const {
    vector<CustomerData> result;
    for (const auto& pair : customerDatabase) {
        if (pair.second.riskScore >= minScore && pair.second.riskScore <= maxScore) {
            result.push_back(pair.second);
        }
    }
    return result;
}

int RiskAssessmentModule::getCustomerCount() const {
    return customerDatabase.size();
}

map<CreditRating, int> RiskAssessmentModule::getRatingDistribution() const {
    map<CreditRating, int> distribution;
    for (const auto& pair : customerDatabase) {
        distribution[pair.second.rating]++;
    }
    return distribution;
}

double RiskAssessmentModule::getAverageRiskScore() const {
    if (customerDatabase.empty()) return 0.0;

    double total = 0.0;
    for (const auto& pair : customerDatabase) {
        total += pair.second.riskScore;
    }
    return total / customerDatabase.size();
}

int RiskAssessmentModule::getHighRiskCustomerCount() const {
    int count = 0;
    for (const auto& pair : customerDatabase) {
        if (pair.second.isHighRisk) {
            count++;
        }
    }
    return count;
}

string RiskAssessmentModule::generateAssessmentReport(const string& customerId) const {
    const CustomerData* data = getCustomerData(customerId);
    if (!data) {
        return "Customer not found!";
    }

    stringstream ss;
    ss << "=== Risk Assessment Report ===" << endl;
    ss << "Customer ID: " << data->customerId << endl;
    ss << "Name: " << data->name << endl;
    ss << "ID Card: " << data->idCard << endl;
    ss << "Assessment Time: " << timeToString(data->assessmentTime) << endl;
    ss << "Assessor: " << data->assessor << endl;
    ss << endl;
    ss << "=== Assessment Data ===" << endl;
    ss << "Monthly Income: " << data->monthlyIncome << endl;
    ss << "Credit Score: " << data->creditScore << endl;
    ss << "Debt Ratio: " << (data->debtRatio * 100) << "%" << endl;
    ss << "Age: " << data->age << endl;
    ss << "Employment Status: " << data->employmentStatus << endl;
    ss << endl;
    ss << "=== Assessment Results ===" << endl;
    ss << "Risk Score: " << data->riskScore << "/100" << endl;

    string ratingStr;
    switch (data->rating) {
        case AAA: ratingStr = "AAA (Excellent)"; break;
        case AA: ratingStr = "AA (Very Good)"; break;
        case A: ratingStr = "A (Good)"; break;
        case B: ratingStr = "B (Fair)"; break;
        case C: ratingStr = "C (Poor)"; break;
    }
    ss << "Credit Rating: " << ratingStr << endl;
    ss << "High Risk: " << (data->isHighRisk ? "Yes" : "No") << endl;

    return ss.str();
}