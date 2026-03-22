#ifndef RISK_MODULE_H
#define RISK_MODULE_H

#include "common.h"

enum class CreditLevel {
    AAA,
    AA,
    A,
    B,
    C
};

string creditLevelToString(CreditLevel level);
CreditLevel stringToCreditLevel(const string& str);

struct CustomerData {
    string customerId;
    string customerName;
    string idCard;
    double monthlyIncome;
    double totalAssets;
    double totalDebt;
    int creditScore;
    int loanCount;
    int overdueCount;
    Date registerTime;
    CreditLevel creditLevel;
    double riskScore;
    bool isHighRisk;
    string remark;
    
    CustomerData() : monthlyIncome(0), totalAssets(0), totalDebt(0),
                     creditScore(0), loanCount(0), overdueCount(0),
                     creditLevel(CreditLevel::C), riskScore(0), isHighRisk(false) {}
    
    string toString() const;
    static CustomerData fromString(const string& str);
    
    bool operator<(const CustomerData& other) const {
        return registerTime < other.registerTime;
    }
};

struct DecisionNode {
    string attribute;
    double threshold;
    string decision;
    DecisionNode* left;
    DecisionNode* right;
    
    DecisionNode() : threshold(0), left(nullptr), right(nullptr) {}
    bool isLeaf() const { return left == nullptr && right == nullptr; }
};

class DecisionTree {
private:
    DecisionNode* root;
    
    void deleteTree(DecisionNode* node);
    CreditLevel evaluate(CustomerData& customer, DecisionNode* node);
    void buildDefaultTree();
    
public:
    DecisionTree();
    ~DecisionTree();
    
    void train();
    CreditLevel predict(CustomerData& customer);
    void setRule(const string& attribute, double threshold);
    void displayRules();
    void clear();
};

class RiskAssessment {
private:
    map<string, CustomerData> customers;
    DecisionTree decisionTree;
    
    string customerFile;
    
    double calculateRiskScore(const CustomerData& customer);
    bool checkHighRisk(const CustomerData& customer);
    
public:
    RiskAssessment();
    ~RiskAssessment();
    
    string addCustomer(const string& name, const string& idCard,
                       double monthlyIncome, double totalAssets,
                       double totalDebt, int creditScore,
                       int loanCount, int overdueCount);
    bool updateCustomer(const string& customerId,
                        double monthlyIncome = -1,
                        double totalAssets = -1,
                        double totalDebt = -1,
                        int creditScore = -1,
                        int loanCount = -1,
                        int overdueCount = -1);
    bool deleteCustomer(const string& customerId);
    
    CustomerData* getCustomer(const string& customerId);
    vector<CustomerData> getAllCustomers(SortOrder order = SortOrder::ASC,
                                          const string& sortBy = "level");
    vector<CustomerData> getHighRiskCustomers();
    vector<CustomerData> getByCreditLevel(CreditLevel level);
    
    void assessCustomer(const string& customerId);
    void assessAllCustomers();
    
    void adjustRules();
    void displayRules();
    
    bool loadCustomers();
    bool saveCustomers();
    
    void displayCustomer(const string& customerId);
    void displayAllCustomers();
    void displayStatistics();
    void displayHighRiskCustomers();
};

#endif
