#ifndef QUEUE_MODULE_H
#define QUEUE_MODULE_H

#include "common.h"

enum class CustomerType {
    NORMAL,
    VIP
};

enum class QueueStatus {
    WAITING,
    SERVING,
    COMPLETED,
    TIMEOUT,
    CANCELLED
};

struct QueueNode {
    string ticketId;
    CustomerType type;
    Date takeTime;
    QueueStatus status;
    int windowId;
    string customerName;
    int priority;
    int position;
    
    QueueNode() : type(CustomerType::NORMAL), status(QueueStatus::WAITING), 
                  windowId(-1), priority(0), position(0) {}
    
    string toString() const;
    static QueueNode fromString(const string& str);
};

class CircularQueue {
private:
    static const int MAX_QUEUE_SIZE = 100;
    static const int MAX_WINDOW_COUNT = 5;
    static const int TIMEOUT_MINUTES = 30;
    
    QueueNode* data;
    int front;
    int rear;
    int count;
    int nextNormalNumber;
    int nextVIPNumber;
    
    vector<int> windowStatus;
    
    string dataFile;
    
    void updatePositions();
    void checkTimeout();
    int calculatePriority(const QueueNode& node);
    
public:
    CircularQueue();
    ~CircularQueue();
    
    string takeTicket(const string& name, CustomerType type);
    bool callNext(int windowId);
    bool cancelTicket(const string& ticketId);
    bool upgradeToVIP(const string& ticketId);
    
    int queryPosition(const string& ticketId);
    QueueNode* queryTicket(const string& ticketId);
    
    vector<QueueNode> getAllWaiting(SortOrder order = SortOrder::ASC);
    vector<QueueNode> getByStatus(QueueStatus status);
    
    int getWaitingCount() const;
    int getAvailableWindows() const;
    
    bool loadData();
    bool saveData();
    
    void displayQueue();
    void displayStatistics();
};

#endif
