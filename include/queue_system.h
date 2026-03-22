#ifndef QUEUE_SYSTEM_H
#define QUEUE_SYSTEM_H

#include "common.h"

// 客户类型
enum CustomerType { NORMAL = 0, VIP = 1 };

// 客户状态
enum CustomerStatus { WAITING = 0, SERVING = 1, EXPIRED = 2, COMPLETED = 3 };

// 客户结构体
struct Customer {
    int ticketNumber;
    CustomerType type;
    CustomerStatus status;
    long takeTime;
    long serveTime;
    int windowId;
    
    Customer() : ticketNumber(0), type(NORMAL), status(WAITING), 
                 takeTime(0), serveTime(0), windowId(-1) {}
    
    Customer(int num, CustomerType t) : ticketNumber(num), type(t), 
                                        status(WAITING), takeTime(Utils::getCurrentTimestamp()), 
                                        serveTime(0), windowId(-1) {}
};

// 循环队列实现的排队叫号系统
class QueueSystem : public FileStorage {
private:
    Customer* queue;
    int front;
    int rear;
    int capacity;
    int size;
    int nextTicketNumber;
    
    static const int MAX_QUEUE_SIZE = 100;
    static const int TIMEOUT_MINUTES = 30;
    static const int MAX_CONCURRENT_PER_WINDOW = 5;
    
    int* windowServingCount;
    int windowCount;
    
    bool isFull() const { return size >= capacity; }
    bool isEmpty() const { return size == 0; }
    int nextIndex(int idx) const { return (idx + 1) % capacity; }
    
public:
    QueueSystem(int cap = MAX_QUEUE_SIZE, int windows = 5);
    ~QueueSystem();
    
    // 核心功能
    int takeNumber(CustomerType type);
    bool callNumber(int windowId);
    bool completeService(int ticketNumber);
    bool cancelTicket(int ticketNumber);
    bool upgradeToVip(int ticketNumber);
    
    // 查询功能
    int getQueuePosition(int ticketNumber) const;
    void queryQueueStatus() const;
    void queryByTime() const;
    void queryByPriority() const;
    
    // 管理功能
    void checkTimeout();
    int getQueueSize() const { return size; }
    int getWindowLoad(int windowId) const;
    
    // 文件存储
    bool saveToFile(const string& filename) override;
    bool loadFromFile(const string& filename) override;
    
    // 菜单接口
    void showMenu();
    void run();
};

#endif
