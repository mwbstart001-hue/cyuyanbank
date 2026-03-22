#include "common.h"
#include <random>
#include <filesystem>

namespace fs = filesystem;

string generateId(const string& prefix) {
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(100000, 999999);
    
    ostringstream oss;
    oss << prefix << dis(gen);
    return oss.str();
}

bool fileExists(const string& filename) {
    return fs::exists(filename);
}

vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    istringstream iss(str);
    string token;
    while (getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}
