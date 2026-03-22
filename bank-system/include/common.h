#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <functional>

using namespace std;

enum class SortOrder {
    ASC,
    DESC
};

struct Date {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    
    Date() : year(1970), month(1), day(1), hour(0), minute(0), second(0) {}
    
    Date(int y, int mo, int d, int h = 0, int mi = 0, int s = 0)
        : year(y), month(mo), day(d), hour(h), minute(mi), second(s) {}
    
    static Date now() {
        time_t t = time(nullptr);
        tm* local = localtime(&t);
        return Date(local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                    local->tm_hour, local->tm_min, local->tm_sec);
    }
    
    string toString() const {
        ostringstream oss;
        oss << setfill('0') 
            << year << "-" 
            << setw(2) << month << "-" 
            << setw(2) << day << " "
            << setw(2) << hour << ":" 
            << setw(2) << minute << ":" 
            << setw(2) << second;
        return oss.str();
    }
    
    time_t toTimestamp() const {
        tm t = {};
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = second;
        return mktime(&t);
    }
    
    static Date fromString(const string& str) {
        Date d;
        sscanf(str.c_str(), "%d-%d-%d %d:%d:%d",
               &d.year, &d.month, &d.day, &d.hour, &d.minute, &d.second);
        return d;
    }
    
    bool operator<(const Date& other) const {
        return toTimestamp() < other.toTimestamp();
    }
    
    bool operator==(const Date& other) const {
        return toTimestamp() == other.toTimestamp();
    }
    
    bool operator>(const Date& other) const {
        return toTimestamp() > other.toTimestamp();
    }
    
    int diffMinutes(const Date& other) const {
        return static_cast<int>(difftime(other.toTimestamp(), toTimestamp()) / 60);
    }
};

string generateId(const string& prefix);

bool fileExists(const string& filename);

vector<string> splitString(const string& str, char delimiter);

string trim(const string& str);

#endif
