#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include "common.h"

// 信用等级
enum CreditLevel { AAA = 5, AA = 4, A = 3, B = 2, C = 1 };

// 客户数据结构
struct CustomerData {
    int id;
    string name;
    double income;
    int creditScore;
    double debtRatio;
    int employmentYears;
    bool hasDefaultHistory;
    
    CustomerData() : id(0), income(0), creditScore(0), debtRatio(0), 
                     employmentYears(0), hasDefaultHistory(false) {}
};

// 评估结果
struct EvaluationResult {
    int customerId;
    CreditLevel level;
    int riskScore;
    string evaluationTime;
    bool isHighRisk;
    string remarks;
    
    EvaluationResult() : customerId(0), level(C), riskScore(0), isHighRisk(false) {}
};

// 决策树节点
struct DecisionNode {
    string condition;
    int attributeIndex;
    double threshold;
    bool isLeaf;
    CreditLevel result;
    DecisionNode* left;
    DecisionNode* right;
    
    DecisionNode() : attributeIndex(-1), threshold(0), isLeaf(false), 
                     result(C), left(nullptr), right(nullptr) {}
};

// 决策树实现的风险评估系统
class DecisionTreeSystem : public FileStorage {
private:
    CustomerData* customers;
    int customerCapacity;
    int customerCount;
    
    EvaluationResult* evaluations;
    int evalCapacity;
    int evalCount;
    
    DecisionNode* root;
    
    // 决策规则参数
    struct Rules {
        double incomeThreshold;
        int creditScoreThreshold;
        double debtRatioThreshold;
        int employmentYearsThreshold;
    } rules;
    
    // 决策树操作
    DecisionNode* buildTree();
    void deleteTree(DecisionNode* node);
    CreditLevel evaluateNode(DecisionNode* node, const CustomerData& data);
    int calculateRiskScore(const CustomerData& data);
    
public:
    DecisionTreeSystem(int cap = 1000);
    ~DecisionTreeSystem();
    
    // 客户数据管理
    bool addCustomer(int id, const string& name, double income, int creditScore,
                     double debtRatio, int employmentYears, bool hasDefault);
    bool updateCustomer(int id, const string& field, double value);
    CustomerData* getCustomer(int id);
    void showAllCustomers();
    
    // 风险评估
    EvaluationResult evaluateCustomer(int customerId);
    void evaluateAll();
    void showEvaluation(int customerId);
    void showAllEvaluations();
    
    // 规则调整
    void adjustRules(double income, int credit, double debt, int employment);
    void showCurrentRules();
    
    // 排序查询
    void sortByCreditLevel();
    void sortByRiskScore();
    void showHighRiskCustomers();
    
    // 文件存储
    bool saveToFile(const string& filename) override;
    bool loadFromFile(const string& filename) override;
    bool saveEvaluations(const string& filename);
    bool loadEvaluations(const string& filename);
    
    // 菜单接口
    void showMenu();
    void run();
};

#endif
