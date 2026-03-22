#ifndef QUEUING_SYSTEM_H
#define QUEUING_SYSTEM_H

#include "common.h"

struct Customer {
    int id;
    string name;
    string idCard;
    CustomerType type;
    int priority;
    time_t createTime;
    time_t lastCallTime;
    bool isCalled;
    int windowNumber;

    Customer() : id(0), type(NORMAL), priority(0), createTime(0), lastCallTime(0), isCalled(false), windowNumber(0) {}
};

class CircularQueue {
private:
    vector<Customer> data;
    int front;
    int rear;
    int maxSize;
    int currentSize;

public:
    CircularQueue(int size = 100);
    bool enqueue(const Customer& customer);
    bool dequeue(Customer& customer);
    bool isEmpty() const;
    bool isFull() const;
    int size() const;
    Customer* getFront();
    void clear();
    vector<Customer> getAllElements() const;
};

class QueuingSystem {
private:
    CircularQueue queue;
    int nextCustomerId;
    int maxQueueSize;
    int timeoutMinutes;
    int maxConcurrentWindows;
    map<int, bool> windowStatus;

    void sortQueue();
    void removeExpiredCustomers();

public:
    QueuingSystem(int maxSize = 100, int timeout = 30, int maxWindows = 5);

    // Core functions
    int takeNumber(const string& name, const string& idCard, CustomerType type);
    bool callNumber(int windowNumber, Customer& customer);
    int getQueuePosition(int customerId) const;
    bool upgradeToVIP(int customerId);
    bool cancelNumber(int customerId);

    // Management functions
    void cleanExpired();
    int getCurrentQueueSize() const;
    vector<Customer> getQueueList() const;
    vector<Customer> getSortedByTime() const;
    vector<Customer> getSortedByPriority() const;

    // Statistics
    int getNormalCustomerCount() const;
    int getVIPCustomerCount() const;
    int getCalledCustomerCount() const;
};

#endif // QUEUING_SYSTEM_H