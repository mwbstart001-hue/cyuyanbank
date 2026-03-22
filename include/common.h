#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

using namespace std;

// 通用工具函数
class Utils {
public:
    static string getCurrentTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char buffer[20];
        sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
                1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        return string(buffer);
    }
    
    static long getCurrentTimestamp() {
        return time(0);
    }
    
    static string timestampToString(long timestamp) {
        time_t t = timestamp;
        tm* ltm = localtime(&t);
        char buffer[20];
        sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
                1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
        return string(buffer);
    }
};

// 文件存储基类
class FileStorage {
public:
    virtual bool saveToFile(const string& filename) = 0;
    virtual bool loadFromFile(const string& filename) = 0;
};

#endif
