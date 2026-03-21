#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <string>

/*
 * 哈夫曼树（Huffman Tree）
 * 
 * 【开发者C 负责】
 * 
 * 用途：对银行交易流水日志进行数据压缩。
 * 哈夫曼编码是一种最优的无损压缩算法，
 * 通过为高频字符分配短编码、低频字符分配长编码，
 * 来最小化编码后的总长度。
 * 
 * 改进说明：
 *   - 修复了只有一种字符时崩溃的 BUG
 *   - 新增 decompress() 解压方法，形成完整的压缩-解压闭环
 *   - 增加了内存安全的树销毁逻辑
 */

// 哈夫曼树节点
struct HuffmanNode {
    char data;              // 存储的字符（叶子节点有效）
    int freq;               // 字符出现的频率
    HuffmanNode *left, *right;

    HuffmanNode(char data, int freq)
        : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// 优先队列比较器：频率小的优先出队（构建最小堆）
struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

class Huffman {
private:
    HuffmanNode* root;                       // 哈夫曼树的根节点
    std::map<char, std::string> codes;       // 字符 -> 编码 映射表

    // 递归生成哈夫曼编码
    // 左走加 '0'，右走加 '1'
    void generateCodes(HuffmanNode* node, const std::string& str) {
        if (!node) return;
        // 叶子节点：记录该字符的编码
        if (!node->left && !node->right) {
            // 特殊情况：如果只有一个字符，编码为 "0"
            codes[node->data] = str.empty() ? "0" : str;
            return;
        }
        generateCodes(node->left, str + "0");
        generateCodes(node->right, str + "1");
    }

    // 递归销毁整棵哈夫曼树，释放内存
    void deleteTree(HuffmanNode* node) {
        if (!node) return;
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }

public:
    Huffman() : root(nullptr) {}

    ~Huffman() {
        deleteTree(root);
        root = nullptr;
    }

    // 构建哈夫曼树
    // 步骤：
    //   1. 统计每个字符的出现频率
    //   2. 将所有字符作为叶子节点放入最小堆
    //   3. 每次取出频率最小的两个节点合并，直到只剩一个根节点
    //   4. 从根节点递归生成编码表
    void build(const std::string& text) {
        // 如果之前构建过，先清除旧数据
        if (root) {
            deleteTree(root);
            root = nullptr;
            codes.clear();
        }

        if (text.empty()) return;

        // 第1步：统计字符频率
        std::map<char, int> freq;
        for (char ch : text) freq[ch]++;

        // 第2步：创建最小堆
        std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> minHeap;
        for (auto const& [ch, f] : freq) {
            minHeap.push(new HuffmanNode(ch, f));
        }

        // 特殊情况：只有一种字符
        if (minHeap.size() == 1) {
            root = minHeap.top();
            minHeap.pop();
            codes[root->data] = "0";  // 唯一字符编码为 "0"
            return;
        }

        // 第3步：构建哈夫曼树
        while (minHeap.size() > 1) {
            HuffmanNode *left = minHeap.top(); minHeap.pop();
            HuffmanNode *right = minHeap.top(); minHeap.pop();
            // 创建内部节点，'\0' 表示非叶子
            HuffmanNode *merged = new HuffmanNode('\0', left->freq + right->freq);
            merged->left = left;
            merged->right = right;
            minHeap.push(merged);
        }
        root = minHeap.top();

        // 第4步：生成编码表
        generateCodes(root, "");
    }

    // 压缩：将原始文本转换为二进制编码字符串
    std::string compress(const std::string& text) {
        std::string result;
        for (char ch : text) {
            auto it = codes.find(ch);
            if (it != codes.end()) {
                result += it->second;
            }
        }
        return result;
    }

    // 解压：将二进制编码字符串还原为原始文本
    // 从根节点出发，遇 '0' 走左，遇 '1' 走右，
    // 到达叶子节点时输出该字符并回到根节点继续。
    std::string decompress(const std::string& compressed) {
        if (!root) return "";

        // 特殊情况：只有一个节点（单字符重复）
        if (!root->left && !root->right) {
            std::string result;
            for (size_t i = 0; i < compressed.size(); i++) {
                result += root->data;
            }
            return result;
        }

        std::string result;
        HuffmanNode* current = root;
        for (char bit : compressed) {
            if (bit == '0') {
                current = current->left;
            } else {
                current = current->right;
            }
            // 到达叶子节点
            if (!current->left && !current->right) {
                result += current->data;
                current = root;  // 回到根节点继续解码
            }
        }
        return result;
    }

    // 获取编码表（供外部查看各字符的编码）
    std::map<char, std::string> getCodes() const { return codes; }
};

#endif // HUFFMAN_H
