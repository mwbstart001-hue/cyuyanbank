#ifndef RISK_ASSESSMENT_H
#define RISK_ASSESSMENT_H

#include "common.h"

struct CustomerData {
    string customerId;
    string name;
    string idCard;
    double monthlyIncome;
    int creditScore;
    double debtRatio;
    int age;
    string employmentStatus;
    time_t assessmentTime;
    CreditRating rating;
    int riskScore;
    bool isHighRisk;
    string assessor;

    CustomerData() : monthlyIncome(0.0), creditScore(0), debtRatio(0.0), age(0),
                     assessmentTime(0), rating(C), riskScore(0), isHighRisk(false) {}
};

struct AssessmentRule {
    string ruleId;
    string dimension;
    double minValue;
    double maxValue;
    int score;
    string description;
    bool isEnabled;

    AssessmentRule() : minValue(0.0), maxValue(0.0), score(0), isEnabled(true) {}
};

struct DecisionTreeNode {
    string attribute;
    double threshold;
    int score;
    CreditRating rating;
    DecisionTreeNode* left;
    DecisionTreeNode* right;
    bool isLeaf;

    DecisionTreeNode() : threshold(0.0), score(0), rating(C), left(nullptr), right(nullptr), isLeaf(false) {}
};

class DecisionTree {
private:
    DecisionTreeNode* root;
    vector<AssessmentRule> rules;

    void destroyTree(DecisionTreeNode* node);
    int evaluateNode(DecisionTreeNode* node, const CustomerData& data) const;

public:
    DecisionTree();
    ~DecisionTree();

    void buildTree(const vector<AssessmentRule>& assessmentRules);
    int evaluate(const CustomerData& data) const;
    CreditRating getRating(int totalScore) const;
    void updateRules(const vector<AssessmentRule>& newRules);
    vector<AssessmentRule> getRules() const;
};

class RiskAssessmentModule {
private:
    map<string, CustomerData> customerDatabase;
    DecisionTree decisionTree;
    vector<AssessmentRule> assessmentRules;

    void initializeDefaultRules();
    int calculateRiskScore(const CustomerData& data) const;
    CreditRating determineRating(int riskScore) const;
    bool isHighRisk(CreditRating rating) const;

public:
    RiskAssessmentModule();

    // Customer data management
    bool addCustomerData(const CustomerData& data);
    bool updateCustomerData(const string& customerId, const CustomerData& newData);
    CustomerData* getCustomerData(const string& customerId);
    const CustomerData* getCustomerData(const string& customerId) const;
    vector<CustomerData> getAllCustomers() const;

    // Risk assessment
    bool assessCustomer(const string& customerId, const string& assessor = "SYSTEM");
    void batchAssessment();

    // Rule management
    void updateAssessmentRules(const vector<AssessmentRule>& rules);
    vector<AssessmentRule> getAssessmentRules() const;
    bool addRule(const AssessmentRule& rule);
    bool removeRule(const string& ruleId);
    bool enableRule(const string& ruleId, bool enable);

    // Query and sorting
    vector<CustomerData> getSortedByCreditRating(bool ascending = true) const;
    vector<CustomerData> getSortedByRiskScore(bool ascending = false) const;
    vector<CustomerData> getHighRiskCustomers() const;
    vector<CustomerData> queryByRating(CreditRating rating) const;
    vector<CustomerData> queryByRiskScoreRange(int minScore, int maxScore) const;

    // Statistics
    int getCustomerCount() const;
    map<CreditRating, int> getRatingDistribution() const;
    double getAverageRiskScore() const;
    int getHighRiskCustomerCount() const;

    // Report generation
    string generateAssessmentReport(const string& customerId) const;
};

#endif // RISK_ASSESSMENT_H