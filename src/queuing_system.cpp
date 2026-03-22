#include "queuing_system.h"

// CircularQueue implementation
CircularQueue::CircularQueue(int size) : maxSize(size), front(0), rear(0), currentSize(0) {
    data.resize(maxSize);
}

bool CircularQueue::enqueue(const Customer& customer) {
    if (isFull()) return false;
    data[rear] = customer;
    rear = (rear + 1) % maxSize;
    currentSize++;
    return true;
}

bool CircularQueue::dequeue(Customer& customer) {
    if (isEmpty()) return false;
    customer = data[front];
    front = (front + 1) % maxSize;
    currentSize--;
    return true;
}

bool CircularQueue::isEmpty() const {
    return currentSize == 0;
}

bool CircularQueue::isFull() const {
    return currentSize == maxSize;
}

int CircularQueue::size() const {
    return currentSize;
}

Customer* CircularQueue::getFront() {
    if (isEmpty()) return nullptr;
    return &data[front];
}

void CircularQueue::clear() {
    front = rear = currentSize = 0;
}

vector<Customer> CircularQueue::getAllElements() const {
    vector<Customer> result;
    int idx = front;
    for (int i = 0; i < currentSize; i++) {
        result.push_back(data[idx]);
        idx = (idx + 1) % maxSize;
    }
    return result;
}

// QueuingSystem implementation
QueuingSystem::QueuingSystem(int maxSize, int timeout, int maxWindows)
    : queue(maxSize), nextCustomerId(1), maxQueueSize(maxSize), timeoutMinutes(timeout), maxConcurrentWindows(maxWindows) {
    for (int i = 1; i <= maxWindows; i++) {
        windowStatus[i] = false; // false = available
    }
}

void QueuingSystem::sortQueue() {
    vector<Customer> elements = queue.getAllElements();
    queue.clear();

    sort(elements.begin(), elements.end(), [](const Customer& a, const Customer& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        if (a.createTime != b.createTime) {
            return a.createTime < b.createTime;
        }
        return a.id < b.id;
    });

    for (const auto& cust : elements) {
        queue.enqueue(cust);
    }
}

void QueuingSystem::removeExpiredCustomers() {
    vector<Customer> elements = queue.getAllElements();
    queue.clear();
    time_t now = time(nullptr);

    for (const auto& cust : elements) {
        double diff = difftime(now, cust.createTime);
        if (diff <= timeoutMinutes * 60) {
            queue.enqueue(cust);
        }
    }
}

int QueuingSystem::takeNumber(const string& name, const string& idCard, CustomerType type) {
    removeExpiredCustomers();

    if (queue.isFull()) return -1;

    Customer cust;
    cust.id = nextCustomerId++;
    cust.name = name;
    cust.idCard = idCard;
    cust.type = type;
    cust.priority = (type == VIP) ? 10 : 0;
    cust.createTime = time(nullptr);
    cust.isCalled = false;

    if (queue.enqueue(cust)) {
        sortQueue();
        return cust.id;
    }
    return -1;
}

bool QueuingSystem::callNumber(int windowNumber, Customer& customer) {
    removeExpiredCustomers();

    if (windowStatus.find(windowNumber) == windowStatus.end()) {
        return false;
    }

    if (windowStatus[windowNumber]) {
        return false;
    }

    Customer* frontCust = queue.getFront();
    if (!frontCust) return false;

    if (queue.dequeue(customer)) {
        customer.isCalled = true;
        customer.windowNumber = windowNumber;
        customer.lastCallTime = time(nullptr);
        windowStatus[windowNumber] = true;
        return true;
    }
    return false;
}

int QueuingSystem::getQueuePosition(int customerId) const {
    vector<Customer> elements = queue.getAllElements();
    for (size_t i = 0; i < elements.size(); i++) {
        if (elements[i].id == customerId) {
            return i + 1;
        }
    }
    return -1;
}

bool QueuingSystem::upgradeToVIP(int customerId) {
    vector<Customer> elements = queue.getAllElements();
    queue.clear();

    bool found = false;
    for (auto& cust : elements) {
        if (cust.id == customerId && cust.type == NORMAL) {
            cust.type = VIP;
            cust.priority = 10;
            found = true;
        }
        queue.enqueue(cust);
    }

    if (found) {
        sortQueue();
    }
    return found;
}

bool QueuingSystem::cancelNumber(int customerId) {
    vector<Customer> elements = queue.getAllElements();
    queue.clear();

    bool found = false;
    for (const auto& cust : elements) {
        if (cust.id != customerId) {
            queue.enqueue(cust);
        } else {
            found = true;
        }
    }
    return found;
}

void QueuingSystem::cleanExpired() {
    removeExpiredCustomers();
}

int QueuingSystem::getCurrentQueueSize() const {
    return queue.size();
}

vector<Customer> QueuingSystem::getQueueList() const {
    return queue.getAllElements();
}

vector<Customer> QueuingSystem::getSortedByTime() const {
    vector<Customer> result = queue.getAllElements();
    sort(result.begin(), result.end(), [](const Customer& a, const Customer& b) {
        return a.createTime < b.createTime;
    });
    return result;
}

vector<Customer> QueuingSystem::getSortedByPriority() const {
    vector<Customer> result = queue.getAllElements();
    sort(result.begin(), result.end(), [](const Customer& a, const Customer& b) {
        if (a.priority != b.priority) {
            return a.priority > b.priority;
        }
        return a.createTime < b.createTime;
    });
    return result;
}

int QueuingSystem::getNormalCustomerCount() const {
    vector<Customer> elements = queue.getAllElements();
    int count = 0;
    for (const auto& cust : elements) {
        if (cust.type == NORMAL) count++;
    }
    return count;
}

int QueuingSystem::getVIPCustomerCount() const {
    vector<Customer> elements = queue.getAllElements();
    int count = 0;
    for (const auto& cust : elements) {
        if (cust.type == VIP) count++;
    }
    return count;
}

int QueuingSystem::getCalledCustomerCount() const {
    vector<Customer> elements = queue.getAllElements();
    int count = 0;
    for (const auto& cust : elements) {
        if (cust.isCalled) count++;
    }
    return count;
}