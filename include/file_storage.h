#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H

#include "common.h"
#include "queuing_system.h"
#include "account_relationship.h"
#include "transaction_flow.h"
#include "risk_assessment.h"

class FileStorage {
private:
    string dataDirectory;

    bool ensureDirectoryExists(const string& path);

    // Serialization helpers
    string serializeCustomer(const Customer& cust);
    Customer deserializeCustomer(const string& data);
    string serializeAccount(const Account& acc);
    Account deserializeAccount(const string& data);
    string serializeTransaction(const Transaction& trans);
    Transaction deserializeTransaction(const string& data);
    string serializeCustomerData(const CustomerData& data);
    CustomerData deserializeCustomerData(const string& data);
    string serializeRelationshipLog(const RelationshipLog& log);
    RelationshipLog deserializeRelationshipLog(const string& data);
    string serializeAssessmentRule(const AssessmentRule& rule);
    AssessmentRule deserializeAssessmentRule(const string& data);

public:
    FileStorage(const string& dir = "data");

    // Queuing system storage
    bool saveQueuingSystem(const QueuingSystem& system);
    bool loadQueuingSystem(QueuingSystem& system);

    // Account relationship storage
    bool saveAccountRelationship(const AccountRelationshipModule& module);
    bool loadAccountRelationship(AccountRelationshipModule& module);

    // Transaction flow storage
    bool saveTransactionFlow(const TransactionFlowModule& module);
    bool loadTransactionFlow(TransactionFlowModule& module);

    // Risk assessment storage
    bool saveRiskAssessment(const RiskAssessmentModule& module);
    bool loadRiskAssessment(RiskAssessmentModule& module);

    // Full system backup/restore
    bool saveAllData(const QueuingSystem& queuing,
                     const AccountRelationshipModule& accounts,
                     const TransactionFlowModule& transactions,
                     const RiskAssessmentModule& risk);
    bool loadAllData(QueuingSystem& queuing,
                     AccountRelationshipModule& accounts,
                     TransactionFlowModule& transactions,
                     RiskAssessmentModule& risk);

    // File management
    bool backupData(const string& backupName);
    bool restoreBackup(const string& backupName);
    vector<string> listBackups() const;
    bool deleteBackup(const string& backupName);

    // Export/Import
    bool exportToCSV(const string& moduleName, const string& filename);
    bool importFromCSV(const string& moduleName, const string& filename);
};

#endif // FILE_STORAGE_H