#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <unordered_map>

/*
 * 并查集（Union-Find / Disjoint Set Union）
 * 
 * 【开发者D 负责】
 * 
 * 用途：管理账户之间的关联关系网络。
 * 当两个账户发生转账时，它们被视为"关联"账户。
 * 通过并查集，可以高效判断任意两个账户之间是否存在
 * 直接或间接的资金往来关系（风险联查）。
 * 
 * 核心优化：
 *   - 路径压缩（Path Compression）：find 时将节点直接挂到根节点
 *   - 按秩合并（Union by Rank）：合并时将矮树挂到高树上
 * 
 * 改进说明：
 *   使用 unordered_map 替代固定大小 vector，
 *   以适应 SQLite 自增 ID（ID 可能非连续、可能很大），
 *   避免数组越界崩溃。
 */
class UnionFind {
private:
    std::unordered_map<int, int> parent;  // 每个节点的父节点
    std::unordered_map<int, int> rnk;     // 每个节点的秩（树高度的上界）

    // 确保节点存在于并查集中，若不存在则初始化（自己是自己的父节点）
    void ensureNode(int id) {
        if (parent.find(id) == parent.end()) {
            parent[id] = id;
            rnk[id] = 0;
        }
    }

public:
    UnionFind() = default;

    // 查找根节点（带路径压缩）
    // 路径压缩：将查找路径上的所有节点直接连接到根节点，
    // 使得后续查找更快（近似 O(1)）。
    int find(int i) {
        ensureNode(i);
        if (parent[i] == i)
            return i;
        parent[i] = find(parent[i]);  // 递归压缩路径
        return parent[i];
    }

    // 合并两个集合（按秩合并）
    // 将秩较小的树挂到秩较大的树上，保持树的平衡。
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            if (rnk[root_i] < rnk[root_j]) {
                parent[root_i] = root_j;
            } else if (rnk[root_i] > rnk[root_j]) {
                parent[root_j] = root_i;
            } else {
                parent[root_i] = root_j;
                rnk[root_j]++;
            }
        }
    }

    // 判断两个节点是否连通（属于同一集合）
    bool isConnected(int i, int j) {
        return find(i) == find(j);
    }
};

#endif // UNION_FIND_H
