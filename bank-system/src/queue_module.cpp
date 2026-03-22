#include "queue_module.h"

string QueueNode::toString() const {
    ostringstream oss;
    oss << ticketId << "|"
        << (type == CustomerType::VIP ? "VIP" : "NORMAL") << "|"
        << takeTime.toString() << "|"
        << static_cast<int>(status) << "|"
        << windowId << "|"
        << customerName << "|"
        << priority << "|"
        << position;
    return oss.str();
}

QueueNode QueueNode::fromString(const string& str) {
    QueueNode node;
    vector<string> parts = splitString(str, '|');
    if (parts.size() >= 8) {
        node.ticketId = parts[0];
        node.type = (parts[1] == "VIP") ? CustomerType::VIP : CustomerType::NORMAL;
        node.takeTime = Date::fromString(parts[2]);
        node.status = static_cast<QueueStatus>(stoi(parts[3]));
        node.windowId = stoi(parts[4]);
        node.customerName = parts[5];
        node.priority = stoi(parts[6]);
        node.position = stoi(parts[7]);
    }
    return node;
}

CircularQueue::CircularQueue() 
    : front(0), rear(0), count(0), nextNormalNumber(1), nextVIPNumber(1),
      dataFile("data/queue_data.txt") {
    data = new QueueNode[MAX_QUEUE_SIZE];
    windowStatus.resize(MAX_WINDOW_COUNT, 0);
    loadData();
}

CircularQueue::~CircularQueue() {
    saveData();
    delete[] data;
}

int CircularQueue::calculatePriority(const QueueNode& node) {
    int basePriority = (node.type == CustomerType::VIP) ? 100 : 0;
    int waitTime = Date::now().diffMinutes(node.takeTime);
    return basePriority + waitTime;
}

void CircularQueue::updatePositions() {
    int pos = 1;
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == QueueStatus::WAITING) {
            data[idx].position = pos++;
            data[idx].priority = calculatePriority(data[idx]);
        }
    }
}

void CircularQueue::checkTimeout() {
    Date now = Date::now();
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == QueueStatus::WAITING) {
            int waitMinutes = now.diffMinutes(data[idx].takeTime);
            if (waitMinutes > TIMEOUT_MINUTES) {
                data[idx].status = QueueStatus::TIMEOUT;
            }
        }
    }
}

string CircularQueue::takeTicket(const string& name, CustomerType type) {
    if (count >= MAX_QUEUE_SIZE) {
        return "";
    }
    
    checkTimeout();
    
    string prefix = (type == CustomerType::VIP) ? "V" : "N";
    int number = (type == CustomerType::VIP) ? nextVIPNumber++ : nextNormalNumber++;
    
    ostringstream oss;
    oss << prefix << setw(4) << setfill('0') << number;
    string ticketId = oss.str();
    
    QueueNode node;
    node.ticketId = ticketId;
    node.type = type;
    node.takeTime = Date::now();
    node.status = QueueStatus::WAITING;
    node.customerName = name;
    node.windowId = -1;
    node.priority = calculatePriority(node);
    
    data[rear] = node;
    rear = (rear + 1) % MAX_QUEUE_SIZE;
    count++;
    
    updatePositions();
    saveData();
    
    return ticketId;
}

bool CircularQueue::callNext(int windowId) {
    if (windowId < 0 || windowId >= MAX_WINDOW_COUNT) {
        return false;
    }
    
    checkTimeout();
    
    int bestIdx = -1;
    int bestPriority = -1;
    
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == QueueStatus::WAITING) {
            int priority = calculatePriority(data[idx]);
            if (priority > bestPriority) {
                bestPriority = priority;
                bestIdx = idx;
            }
        }
    }
    
    if (bestIdx == -1) {
        return false;
    }
    
    data[bestIdx].status = QueueStatus::SERVING;
    data[bestIdx].windowId = windowId;
    windowStatus[windowId] = 1;
    
    updatePositions();
    saveData();
    
    cout << "\n=== 叫号成功 ===" << endl;
    cout << "票号: " << data[bestIdx].ticketId << endl;
    cout << "客户: " << data[bestIdx].customerName << endl;
    cout << "类型: " << (data[bestIdx].type == CustomerType::VIP ? "VIP" : "普通") << endl;
    cout << "窗口: " << windowId + 1 << endl;
    
    return true;
}

bool CircularQueue::cancelTicket(const string& ticketId) {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].ticketId == ticketId && 
            data[idx].status == QueueStatus::WAITING) {
            data[idx].status = QueueStatus::CANCELLED;
            updatePositions();
            saveData();
            return true;
        }
    }
    return false;
}

bool CircularQueue::upgradeToVIP(const string& ticketId) {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].ticketId == ticketId && 
            data[idx].type == CustomerType::NORMAL &&
            data[idx].status == QueueStatus::WAITING) {
            data[idx].type = CustomerType::VIP;
            
            string newId = "V" + ticketId.substr(1);
            data[idx].ticketId = newId;
            data[idx].priority = calculatePriority(data[idx]);
            
            updatePositions();
            saveData();
            return true;
        }
    }
    return false;
}

int CircularQueue::queryPosition(const string& ticketId) {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].ticketId == ticketId) {
            if (data[idx].status == QueueStatus::WAITING) {
                return data[idx].position;
            }
            return -1;
        }
    }
    return -1;
}

QueueNode* CircularQueue::queryTicket(const string& ticketId) {
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].ticketId == ticketId) {
            return &data[idx];
        }
    }
    return nullptr;
}

vector<QueueNode> CircularQueue::getAllWaiting(SortOrder order) {
    vector<QueueNode> result;
    checkTimeout();
    
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == QueueStatus::WAITING) {
            result.push_back(data[idx]);
        }
    }
    
    if (order == SortOrder::ASC) {
        sort(result.begin(), result.end(), 
             [](const QueueNode& a, const QueueNode& b) -> bool {
                 return a.priority > b.priority;
             });
    } else {
        sort(result.begin(), result.end(), 
             [](const QueueNode& a, const QueueNode& b) -> bool {
                 return a.takeTime < b.takeTime;
             });
    }
    
    return result;
}

vector<QueueNode> CircularQueue::getByStatus(QueueStatus status) {
    vector<QueueNode> result;
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == status) {
            result.push_back(data[idx]);
        }
    }
    return result;
}

int CircularQueue::getWaitingCount() const {
    int cnt = 0;
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        if (data[idx].status == QueueStatus::WAITING) {
            cnt++;
        }
    }
    return cnt;
}

int CircularQueue::getAvailableWindows() const {
    int cnt = 0;
    for (int i = 0; i < MAX_WINDOW_COUNT; i++) {
        if (windowStatus[i] == 0) cnt++;
    }
    return cnt;
}

bool CircularQueue::loadData() {
    ifstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        QueueNode node = QueueNode::fromString(line);
        data[rear] = node;
        rear = (rear + 1) % MAX_QUEUE_SIZE;
        count++;
        
        if (node.type == CustomerType::VIP) {
            int num = stoi(node.ticketId.substr(1));
            if (num >= nextVIPNumber) nextVIPNumber = num + 1;
        } else {
            int num = stoi(node.ticketId.substr(1));
            if (num >= nextNormalNumber) nextNormalNumber = num + 1;
        }
    }
    
    file.close();
    updatePositions();
    return true;
}

bool CircularQueue::saveData() {
    ofstream file(dataFile);
    if (!file.is_open()) {
        return false;
    }
    
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        file << data[idx].toString() << "\n";
    }
    
    file.close();
    return true;
}

void CircularQueue::displayQueue() {
    checkTimeout();
    cout << "\n========== 当前排队信息 ==========" << endl;
    cout << "总排队人数: " << count << endl;
    cout << "等待人数: " << getWaitingCount() << endl;
    cout << "可用窗口: " << getAvailableWindows() << "/" << MAX_WINDOW_COUNT << endl;
    
    cout << "\n--- 等待中的客户 ---" << endl;
    vector<QueueNode> waiting = getAllWaiting();
    if (waiting.empty()) {
        cout << "暂无等待客户" << endl;
    } else {
        cout << left << setw(10) << "票号" 
             << setw(10) << "类型" 
             << setw(15) << "客户姓名"
             << setw(20) << "取号时间"
             << setw(8) << "位置" << endl;
        cout << string(63, '-') << endl;
        
        for (const auto& node : waiting) {
            cout << left << setw(10) << node.ticketId
                 << setw(10) << (node.type == CustomerType::VIP ? "VIP" : "普通")
                 << setw(15) << node.customerName
                 << setw(20) << node.takeTime.toString()
                 << setw(8) << node.position << endl;
        }
    }
}

void CircularQueue::displayStatistics() {
    cout << "\n========== 排队统计信息 ==========" << endl;
    
    int waiting = 0, serving = 0, completed = 0, timeout = 0, cancelled = 0;
    int vipCount = 0, normalCount = 0;
    
    for (int i = 0; i < count; i++) {
        int idx = (front + i) % MAX_QUEUE_SIZE;
        switch (data[idx].status) {
            case QueueStatus::WAITING: waiting++; break;
            case QueueStatus::SERVING: serving++; break;
            case QueueStatus::COMPLETED: completed++; break;
            case QueueStatus::TIMEOUT: timeout++; break;
            case QueueStatus::CANCELLED: cancelled++; break;
        }
        if (data[idx].type == CustomerType::VIP) vipCount++;
        else normalCount++;
    }
    
    cout << "等待中: " << waiting << endl;
    cout << "服务中: " << serving << endl;
    cout << "已完成: " << completed << endl;
    cout << "已超时: " << timeout << endl;
    cout << "已取消: " << cancelled << endl;
    cout << "VIP客户: " << vipCount << endl;
    cout << "普通客户: " << normalCount << endl;
}
