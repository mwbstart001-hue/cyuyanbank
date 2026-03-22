#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <fstream>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <climits>

using namespace std;

// Customer types
enum CustomerType {
    NORMAL,
    VIP
};

// Account status
enum AccountStatus {
    ACTIVE,
    INACTIVE,
    CLOSED
};

// Transaction type
enum TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER,
    PAYMENT
};

// Credit rating
enum CreditRating {
    AAA,
    AA,
    A,
    B,
    C
};

// Common utility functions
string getCurrentTime();
string timeToString(time_t t);
time_t stringToTime(const string& s);
int getRandomNumber(int min, int max);

#endif // COMMON_H