#include "../include/queue_system.h"
#include <cstring>

QueueSystem::QueueSystem(int cap, int windows) {
    capacity = cap;
    queue = new Customer[capacity];
    front = 0;
    rear = -1;
    size = 0;
    nextTicketNumber = 1;
    windowCount = windows;
    windowServingCount = new int[windowCount];
    for (int i = 0; i < windowCount; i++) {
        windowServingCount[i] = 0;
    }
}

QueueSystem::~QueueSystem() {
    delete[] queue;
    delete[] windowServingCount;
}

int QueueSystem::takeNumber(CustomerType type) {
    if (isFull()) {
        cout << "排队已满，请稍后再试！" << endl;
        return -1;
    }
    
    rear = nextIndex(rear);
    queue[rear] = Customer(nextTicketNumber, type);
    size++;
    
    string typeStr = (type == VIP) ? "VIP" : "普通";
    cout << "取号成功！您的" << typeStr << "号码是: " << nextTicketNumber << endl;
    cout << "当前排队人数: " << size << endl;
    
    return nextTicketNumber++;
}

bool QueueSystem::callNumber(int windowId) {
    if (windowId < 0 || windowId >= windowCount) {
        cout << "无效的窗口号！" << endl;
        return false;
    }
    
    if (windowServingCount[windowId] >= MAX_CONCURRENT_PER_WINDOW) {
        cout << "窗口 " << windowId << " 已达到并发限制！" << endl;
        return false;
    }
    
    checkTimeout();
    
    // 优先叫VIP号
    int idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].status == WAITING && queue[idx].type == VIP) {
            queue[idx].status = SERVING;
            queue[idx].serveTime = Utils::getCurrentTimestamp();
            queue[idx].windowId = windowId;
            windowServingCount[windowId]++;
            
            cout << "请VIP客户 " << queue[idx].ticketNumber << " 号到 " << windowId << " 号窗口办理" << endl;
            return true;
        }
        idx = nextIndex(idx);
    }
    
    // 叫普通号
    idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].status == WAITING) {
            queue[idx].status = SERVING;
            queue[idx].serveTime = Utils::getCurrentTimestamp();
            queue[idx].windowId = windowId;
            windowServingCount[windowId]++;
            
            cout << "请普通客户 " << queue[idx].ticketNumber << " 号到 " << windowId << " 号窗口办理" << endl;
            return true;
        }
        idx = nextIndex(idx);
    }
    
    cout << "当前没有等待的客户！" << endl;
    return false;
}

bool QueueSystem::completeService(int ticketNumber) {
    int idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].ticketNumber == ticketNumber && queue[idx].status == SERVING) {
            queue[idx].status = COMPLETED;
            if (queue[idx].windowId >= 0) {
                windowServingCount[queue[idx].windowId]--;
            }
            
            // 从队列中移除
            if (idx == front) {
                front = nextIndex(front);
                size--;
            }
            
            cout << "客户 " << ticketNumber << " 业务办理完成！" << endl;
            return true;
        }
        idx = nextIndex(idx);
    }
    cout << "未找到正在服务的客户 " << ticketNumber << endl;
    return false;
}

bool QueueSystem::cancelTicket(int ticketNumber) {
    int idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].ticketNumber == ticketNumber) {
            if (queue[idx].status == SERVING && queue[idx].windowId >= 0) {
                windowServingCount[queue[idx].windowId]--;
            }
            queue[idx].status = EXPIRED;
            
            if (idx == front) {
                front = nextIndex(front);
                size--;
            }
            
            cout << "客户 " << ticketNumber << " 已作废！" << endl;
            return true;
        }
        idx = nextIndex(idx);
    }
    cout << "未找到客户 " << ticketNumber << endl;
    return false;
}

bool QueueSystem::upgradeToVip(int ticketNumber) {
    int idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].ticketNumber == ticketNumber) {
            if (queue[idx].type == VIP) {
                cout << "该客户已经是VIP！" << endl;
                return false;
            }
            queue[idx].type = VIP;
            cout << "客户 " << ticketNumber << " 已升级为VIP！" << endl;
            return true;
        }
        idx = nextIndex(idx);
    }
    cout << "未找到客户 " << ticketNumber << endl;
    return false;
}

int QueueSystem::getQueuePosition(int ticketNumber) const {
    int idx = front;
    int position = 1;
    for (int i = 0; i < size; i++) {
        if (queue[idx].ticketNumber == ticketNumber) {
            if (queue[idx].status == EXPIRED) {
                return -1;
            }
            return position;
        }
        if (queue[idx].status != EXPIRED) {
            position++;
        }
        idx = nextIndex(idx);
    }
    return -1;
}

void QueueSystem::queryQueueStatus() const {
    cout << "\n========== 排队状态 ==========" << endl;
    cout << "当前排队人数: " << size << endl;
    cout << "窗口服务情况:" << endl;
    for (int i = 0; i < windowCount; i++) {
        cout << "  窗口 " << i << ": " << windowServingCount[i] << "/" << MAX_CONCURRENT_PER_WINDOW << endl;
    }
    
    cout << "\n排队详情:" << endl;
    int idx = front;
    for (int i = 0; i < size; i++) {
        string typeStr = (queue[idx].type == VIP) ? "VIP" : "普通";
        string statusStr;
        switch (queue[idx].status) {
            case WAITING: statusStr = "等待中"; break;
            case SERVING: statusStr = "办理中"; break;
            case EXPIRED: statusStr = "已作废"; break;
            case COMPLETED: statusStr = "已完成"; break;
        }
        cout << "  号码: " << queue[idx].ticketNumber 
             << " [" << typeStr << "] - " << statusStr;
        if (queue[idx].status == SERVING) {
            cout << " (窗口" << queue[idx].windowId << ")";
        }
        cout << endl;
        idx = nextIndex(idx);
    }
    cout << "==============================\n" << endl;
}

void QueueSystem::queryByTime() const {
    cout << "\n========== 按取号时间排序 ==========" << endl;
    
    Customer* sorted = new Customer[size];
    int idx = front;
    for (int i = 0; i < size; i++) {
        sorted[i] = queue[idx];
        idx = nextIndex(idx);
    }
    
    // 冒泡排序
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (sorted[j].takeTime > sorted[j + 1].takeTime) {
                Customer temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < size; i++) {
        string typeStr = (sorted[i].type == VIP) ? "VIP" : "普通";
        cout << "  号码: " << sorted[i].ticketNumber 
             << " [" << typeStr << "] - 取号时间: " 
             << Utils::timestampToString(sorted[i].takeTime) << endl;
    }
    
    delete[] sorted;
    cout << "====================================\n" << endl;
}

void QueueSystem::queryByPriority() const {
    cout << "\n========== 按优先级排序 ==========" << endl;
    
    Customer* sorted = new Customer[size];
    int idx = front;
    for (int i = 0; i < size; i++) {
        sorted[i] = queue[idx];
        idx = nextIndex(idx);
    }
    
    // 按类型(VIP优先)和时间排序
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (sorted[j].type < sorted[j + 1].type ||
                (sorted[j].type == sorted[j + 1].type && 
                 sorted[j].takeTime > sorted[j + 1].takeTime)) {
                Customer temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < size; i++) {
        string typeStr = (sorted[i].type == VIP) ? "VIP" : "普通";
        cout << "  号码: " << sorted[i].ticketNumber 
             << " [" << typeStr << "] - 取号时间: " 
             << Utils::timestampToString(sorted[i].takeTime) << endl;
    }
    
    delete[] sorted;
    cout << "==================================\n" << endl;
}

void QueueSystem::checkTimeout() {
    long currentTime = Utils::getCurrentTimestamp();
    int idx = front;
    for (int i = 0; i < size; i++) {
        if (queue[idx].status == WAITING) {
            long waitMinutes = (currentTime - queue[idx].takeTime) / 60;
            if (waitMinutes >= TIMEOUT_MINUTES) {
                queue[idx].status = EXPIRED;
                cout << "客户 " << queue[idx].ticketNumber << " 等待超时，已自动作废！" << endl;
            }
        }
        idx = nextIndex(idx);
    }
}

int QueueSystem::getWindowLoad(int windowId) const {
    if (windowId >= 0 && windowId < windowCount) {
        return windowServingCount[windowId];
    }
    return -1;
}

bool QueueSystem::saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) return false;
    
    file << nextTicketNumber << endl;
    file << size << endl;
    
    int idx = front;
    for (int i = 0; i < size; i++) {
        file << queue[idx].ticketNumber << " "
             << queue[idx].type << " "
             << queue[idx].status << " "
             << queue[idx].takeTime << " "
             << queue[idx].serveTime << " "
             << queue[idx].windowId << endl;
        idx = nextIndex(idx);
    }
    
    file.close();
    return true;
}

bool QueueSystem::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    file >> nextTicketNumber;
    file >> size;
    
    front = 0;
    rear = size - 1;
    
    for (int i = 0; i < size; i++) {
        int type, status;
        file >> queue[i].ticketNumber
             >> type
             >> status
             >> queue[i].takeTime
             >> queue[i].serveTime
             >> queue[i].windowId;
        queue[i].type = static_cast<CustomerType>(type);
        queue[i].status = static_cast<CustomerStatus>(status);
    }
    
    file.close();
    return true;
}

void QueueSystem::showMenu() {
    cout << "\n========== 排队叫号系统 ==========" << endl;
    cout << "1. 普通客户取号" << endl;
    cout << "2. VIP客户取号" << endl;
    cout << "3. 叫号" << endl;
    cout << "4. 完成服务" << endl;
    cout << "5. 作废号码" << endl;
    cout << "6. 查询排队位置" << endl;
    cout << "7. 普通转VIP" << endl;
    cout << "8. 查看排队状态" << endl;
    cout << "9. 按取号时间排序" << endl;
    cout << "10. 按优先级排序" << endl;
    cout << "11. 保存数据" << endl;
    cout << "12. 加载数据" << endl;
    cout << "0. 返回主菜单" << endl;
    cout << "==================================" << endl;
    cout << "请选择: ";
}

void QueueSystem::run() {
    int choice;
    do {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1: {
                takeNumber(NORMAL);
                break;
            }
            case 2: {
                takeNumber(VIP);
                break;
            }
            case 3: {
                int windowId;
                cout << "请输入窗口号: ";
                cin >> windowId;
                callNumber(windowId);
                break;
            }
            case 4: {
                int ticketNumber;
                cout << "请输入完成的号码: ";
                cin >> ticketNumber;
                completeService(ticketNumber);
                break;
            }
            case 5: {
                int ticketNumber;
                cout << "请输入要作废的号码: ";
                cin >> ticketNumber;
                cancelTicket(ticketNumber);
                break;
            }
            case 6: {
                int ticketNumber;
                cout << "请输入要查询的号码: ";
                cin >> ticketNumber;
                int pos = getQueuePosition(ticketNumber);
                if (pos > 0) {
                    cout << "您当前排在第 " << pos << " 位" << endl;
                } else {
                    cout << "未找到该号码或已作废！" << endl;
                }
                break;
            }
            case 7: {
                int ticketNumber;
                cout << "请输入要升级的号码: ";
                cin >> ticketNumber;
                upgradeToVip(ticketNumber);
                break;
            }
            case 8: {
                queryQueueStatus();
                break;
            }
            case 9: {
                queryByTime();
                break;
            }
            case 10: {
                queryByPriority();
                break;
            }
            case 11: {
                if (saveToFile("queue_data.txt")) {
                    cout << "数据保存成功！" << endl;
                } else {
                    cout << "数据保存失败！" << endl;
                }
                break;
            }
            case 12: {
                if (loadFromFile("queue_data.txt")) {
                    cout << "数据加载成功！" << endl;
                } else {
                    cout << "数据加载失败！" << endl;
                }
                break;
            }
            case 0: {
                cout << "返回主菜单..." << endl;
                break;
            }
            default: {
                cout << "无效选择，请重试！" << endl;
            }
        }
    } while (choice != 0);
}
