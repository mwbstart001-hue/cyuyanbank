#include "file_storage.h"
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

FileStorage::FileStorage(const string& dir) : dataDirectory(dir) {
    ensureDirectoryExists(dir);
}

bool FileStorage::ensureDirectoryExists(const string& path) {
    #ifdef _WIN32
        return _mkdir(path.c_str()) == 0 || errno == EEXIST;
    #else
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
    #endif
}

string FileStorage::serializeCustomer(const Customer& cust) {
    stringstream ss;
    ss << cust.id << "|"
       << cust.name << "|"
       << cust.idCard << "|"
       << static_cast<int>(cust.type) << "|"
       << cust.priority << "|"
       << cust.createTime << "|"
       << cust.lastCallTime << "|"
       << (cust.isCalled ? "1" : "0") << "|"
       << cust.windowNumber;
    return ss.str();
}

Customer FileStorage::deserializeCustomer(const string& data) {
    Customer cust;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 9) {
        cust.id = stoi(tokens[0]);
        cust.name = tokens[1];
        cust.idCard = tokens[2];
        cust.type = static_cast<CustomerType>(stoi(tokens[3]));
        cust.priority = stoi(tokens[4]);
        cust.createTime = stoll(tokens[5]);
        cust.lastCallTime = stoll(tokens[6]);
        cust.isCalled = (tokens[7] == "1");
        cust.windowNumber = stoi(tokens[8]);
    }
    return cust;
}

string FileStorage::serializeAccount(const Account& acc) {
    stringstream ss;
    ss << acc.accountId << "|"
       << acc.customerName << "|"
       << acc.idCard << "|"
       << acc.balance << "|"
       << acc.openTime << "|"
       << static_cast<int>(acc.status) << "|"
       << acc.remark;
    return ss.str();
}

Account FileStorage::deserializeAccount(const string& data) {
    Account acc;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 7) {
        acc.accountId = tokens[0];
        acc.customerName = tokens[1];
        acc.idCard = tokens[2];
        acc.balance = stod(tokens[3]);
        acc.openTime = stoll(tokens[4]);
        acc.status = static_cast<AccountStatus>(stoi(tokens[5]));
        acc.remark = tokens[6];
    }
    return acc;
}

string FileStorage::serializeTransaction(const Transaction& trans) {
    stringstream ss;
    ss << trans.transactionId << "|"
       << trans.accountId << "|"
       << static_cast<int>(trans.type) << "|"
       << trans.amount << "|"
       << trans.description << "|"
       << trans.createTime << "|"
       << (trans.isDeleted ? "1" : "0") << "|"
       << trans.operatorId;
    return ss.str();
}

Transaction FileStorage::deserializeTransaction(const string& data) {
    Transaction trans;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 8) {
        trans.transactionId = tokens[0];
        trans.accountId = tokens[1];
        trans.type = static_cast<TransactionType>(stoi(tokens[2]));
        trans.amount = stod(tokens[3]);
        trans.description = tokens[4];
        trans.createTime = stoll(tokens[5]);
        trans.isDeleted = (tokens[6] == "1");
        trans.operatorId = tokens[7];
    }
    return trans;
}

string FileStorage::serializeCustomerData(const CustomerData& data) {
    stringstream ss;
    ss << data.customerId << "|"
       << data.name << "|"
       << data.idCard << "|"
       << data.monthlyIncome << "|"
       << data.creditScore << "|"
       << data.debtRatio << "|"
       << data.age << "|"
       << data.employmentStatus << "|"
       << data.assessmentTime << "|"
       << static_cast<int>(data.rating) << "|"
       << data.riskScore << "|"
       << (data.isHighRisk ? "1" : "0") << "|"
       << data.assessor;
    return ss.str();
}

CustomerData FileStorage::deserializeCustomerData(const string& data) {
    CustomerData cust;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 13) {
        cust.customerId = tokens[0];
        cust.name = tokens[1];
        cust.idCard = tokens[2];
        cust.monthlyIncome = stod(tokens[3]);
        cust.creditScore = stoi(tokens[4]);
        cust.debtRatio = stod(tokens[5]);
        cust.age = stoi(tokens[6]);
        cust.employmentStatus = tokens[7];
        cust.assessmentTime = stoll(tokens[8]);
        cust.rating = static_cast<CreditRating>(stoi(tokens[9]));
        cust.riskScore = stoi(tokens[10]);
        cust.isHighRisk = (tokens[11] == "1");
        cust.assessor = tokens[12];
    }
    return cust;
}

string FileStorage::serializeRelationshipLog(const RelationshipLog& log) {
    stringstream ss;
    ss << log.accountId1 << "|"
       << log.accountId2 << "|"
       << log.operation << "|"
       << log.operationTime << "|"
       << log.operatorId;
    return ss.str();
}

RelationshipLog FileStorage::deserializeRelationshipLog(const string& data) {
    RelationshipLog log;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 5) {
        log.accountId1 = tokens[0];
        log.accountId2 = tokens[1];
        log.operation = tokens[2];
        log.operationTime = stoll(tokens[3]);
        log.operatorId = tokens[4];
    }
    return log;
}

string FileStorage::serializeAssessmentRule(const AssessmentRule& rule) {
    stringstream ss;
    ss << rule.ruleId << "|"
       << rule.dimension << "|"
       << rule.minValue << "|"
       << rule.maxValue << "|"
       << rule.score << "|"
       << rule.description << "|"
       << (rule.isEnabled ? "1" : "0");
    return ss.str();
}

AssessmentRule FileStorage::deserializeAssessmentRule(const string& data) {
    AssessmentRule rule;
    stringstream ss(data);
    string token;
    vector<string> tokens;

    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if (tokens.size() >= 7) {
        rule.ruleId = tokens[0];
        rule.dimension = tokens[1];
        rule.minValue = stod(tokens[2]);
        rule.maxValue = stod(tokens[3]);
        rule.score = stoi(tokens[4]);
        rule.description = tokens[5];
        rule.isEnabled = (tokens[6] == "1");
    }
    return rule;
}

bool FileStorage::saveQueuingSystem(const QueuingSystem& system) {
    string filename = dataDirectory + "/queuing.dat";
    ofstream file(filename);
    if (!file.is_open()) return false;

    vector<Customer> customers = system.getQueueList();
    for (const auto& cust : customers) {
        file << serializeCustomer(cust) << endl;
    }

    file.close();
    return true;
}

bool FileStorage::loadQueuingSystem(QueuingSystem& system) {
    string filename = dataDirectory + "/queuing.dat";
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Customer cust = deserializeCustomer(line);
    }

    file.close();
    return true;
}

bool FileStorage::saveAccountRelationship(const AccountRelationshipModule& module) {
    string accFilename = dataDirectory + "/accounts.dat";
    ofstream accFile(accFilename);
    if (!accFile.is_open()) return false;

    vector<Account> accounts = module.getAllAccounts();
    for (const auto& acc : accounts) {
        accFile << serializeAccount(acc) << endl;
    }
    accFile.close();

    string logFilename = dataDirectory + "/relationship_logs.dat";
    ofstream logFile(logFilename);
    if (!logFile.is_open()) return false;

    vector<RelationshipLog> logs = module.getRelationshipLogs();
    for (const auto& log : logs) {
        logFile << serializeRelationshipLog(log) << endl;
    }
    logFile.close();

    return true;
}

bool FileStorage::loadAccountRelationship(AccountRelationshipModule& module) {
    string accFilename = dataDirectory + "/accounts.dat";
    ifstream accFile(accFilename);
    if (!accFile.is_open()) return false;

    string line;
    while (getline(accFile, line)) {
        if (line.empty()) continue;
        Account acc = deserializeAccount(line);
        module.createAccount(acc.accountId, acc.customerName, acc.idCard, acc.balance);
        Account* loadedAcc = module.getAccount(acc.accountId);
        if (loadedAcc) {
            loadedAcc->openTime = acc.openTime;
            loadedAcc->status = acc.status;
            loadedAcc->remark = acc.remark;
        }
    }
    accFile.close();

    return true;
}

bool FileStorage::saveTransactionFlow(const TransactionFlowModule& module) {
    string filename = dataDirectory + "/transactions.dat";
    ofstream file(filename);
    if (!file.is_open()) return false;

    vector<Transaction> transactions = module.getAllTransactions(true);
    for (const auto& trans : transactions) {
        file << serializeTransaction(trans) << endl;
    }

    file.close();
    return true;
}

bool FileStorage::loadTransactionFlow(TransactionFlowModule& module) {
    string filename = dataDirectory + "/transactions.dat";
    ifstream file(filename);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        Transaction trans = deserializeTransaction(line);
        module.addTransaction(trans);
    }

    file.close();
    return true;
}

bool FileStorage::saveRiskAssessment(const RiskAssessmentModule& module) {
    string custFilename = dataDirectory + "/risk_customers.dat";
    ofstream custFile(custFilename);
    if (!custFile.is_open()) return false;

    vector<CustomerData> customers = module.getAllCustomers();
    for (const auto& cust : customers) {
        custFile << serializeCustomerData(cust) << endl;
    }
    custFile.close();

    string ruleFilename = dataDirectory + "/assessment_rules.dat";
    ofstream ruleFile(ruleFilename);
    if (!ruleFile.is_open()) return false;

    vector<AssessmentRule> rules = module.getAssessmentRules();
    for (const auto& rule : rules) {
        ruleFile << serializeAssessmentRule(rule) << endl;
    }
    ruleFile.close();

    return true;
}

bool FileStorage::loadRiskAssessment(RiskAssessmentModule& module) {
    string custFilename = dataDirectory + "/risk_customers.dat";
    ifstream custFile(custFilename);
    if (!custFile.is_open()) return false;

    string line;
    while (getline(custFile, line)) {
        if (line.empty()) continue;
        CustomerData data = deserializeCustomerData(line);
        module.addCustomerData(data);
    }
    custFile.close();

    string ruleFilename = dataDirectory + "/assessment_rules.dat";
    ifstream ruleFile(ruleFilename);
    if (ruleFile.is_open()) {
        vector<AssessmentRule> rules;
        while (getline(ruleFile, line)) {
            if (line.empty()) continue;
            AssessmentRule rule = deserializeAssessmentRule(line);
            rules.push_back(rule);
        }
        if (!rules.empty()) {
            module.updateAssessmentRules(rules);
        }
        ruleFile.close();
    }

    return true;
}

bool FileStorage::saveAllData(const QueuingSystem& queuing,
                               const AccountRelationshipModule& accounts,
                               const TransactionFlowModule& transactions,
                               const RiskAssessmentModule& risk) {
    return saveQueuingSystem(queuing) &&
           saveAccountRelationship(accounts) &&
           saveTransactionFlow(transactions) &&
           saveRiskAssessment(risk);
}

bool FileStorage::loadAllData(QueuingSystem& queuing,
                               AccountRelationshipModule& accounts,
                               TransactionFlowModule& transactions,
                               RiskAssessmentModule& risk) {
    loadQueuingSystem(queuing);
    loadAccountRelationship(accounts);
    loadTransactionFlow(transactions);
    loadRiskAssessment(risk);
    return true;
}

bool FileStorage::backupData(const string& backupName) {
    string backupDir = dataDirectory + "/backup_" + backupName;
    if (!ensureDirectoryExists(backupDir)) {
        return false;
    }

    string command = "cp -r " + dataDirectory + "/*.dat " + backupDir + "/";
    return system(command.c_str()) == 0;
}

bool FileStorage::restoreBackup(const string& backupName) {
    string backupDir = dataDirectory + "/backup_" + backupName;
    string command = "cp -r " + backupDir + "/*.dat " + dataDirectory + "/";
    return system(command.c_str()) == 0;
}

vector<string> FileStorage::listBackups() const {
    vector<string> backups;
    string path = dataDirectory;

    DIR* dir = opendir(path.c_str());
    if (!dir) return backups;

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (name.find("backup_") == 0) {
            backups.push_back(name.substr(7));
        }
    }

    closedir(dir);
    return backups;
}

bool FileStorage::deleteBackup(const string& backupName) {
    string backupDir = dataDirectory + "/backup_" + backupName;
    string command = "rm -rf " + backupDir;
    return system(command.c_str()) == 0;
}

bool FileStorage::exportToCSV(const string& moduleName, const string& filename) {
    return true;
}

bool FileStorage::importFromCSV(const string& moduleName, const string& filename) {
    return true;
}