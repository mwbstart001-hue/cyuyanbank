#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <vector>
#include <string>
#include <stdexcept>

// 循环队列：用于管理银行排队叫号系统
template <typename T>
class CircularQueue {
private:
    std::vector<T> data;
    int head;
    int tail;
    int capacity;
    int count;

public:
    CircularQueue(int size = 100) : data(size), head(0), tail(0), capacity(size), count(0) {}

    // 入队 (Enqueue)
    bool enqueue(const T& item) {
        if (isFull()) return false;
        data[tail] = item;
        tail = (tail + 1) % capacity;
        count++;
        return true;
    }

    // 出队 (Dequeue)
    T dequeue() {
        if (isEmpty()) throw std::runtime_error("Queue is empty");
        T item = data[head];
        head = (head + 1) % capacity;
        count--;
        return item;
    }

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }
    int size() const { return count; }
};

#endif // CIRCULAR_QUEUE_H
