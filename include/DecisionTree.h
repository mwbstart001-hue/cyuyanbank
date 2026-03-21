#ifndef DECISION_TREE_H
#define DECISION_TREE_H

#include <string>

/*
 * 决策树风险评估模型（Decision Tree Risk Assessment）
 * 
 * 【开发者E 负责】
 * 
 * 用途：基于客户的账户余额和信用分，通过决策树逻辑
 * 对客户进行信用等级划分（AAA / AA / A / B / C / D）。
 * 
 * 决策树结构：
 *                  [信用分]
 *                 /    |    \
 *           >=800  >=700  >=600  <600
 *            /       |      |      \
 *       [余额]   [余额]    C       D
 *       / \       / \
 *   >10万  <=   >5万  <=
 *    AAA   AA    A     B
 * 
 * 改进说明：
 *   - 新增 isHighRisk() 方法，返回 bool，用于业务层拦截判断
 *   - 新增 getRiskLevel() 返回数值等级（1-6），便于排序和比较
 *   - 新增 getDetailedReport() 提供完整的风险评估报告
 */
class RiskAssessment {
public:
    // 评估信用等级（返回等级字符串）
    static std::string evaluate(double balance, int creditScore) {
        if (creditScore >= 800) {
            if (balance > 100000) return "AAA (信用极好，高净值)";
            else return "AA (信用极好)";
        } else if (creditScore >= 700) {
            if (balance > 50000) return "A (信用良好)";
            else return "B (信用中等)";
        } else if (creditScore >= 600) {
            return "C (信用一般)";
        } else {
            return "D (高风险客户)";
        }
    }

    // 判断是否为高风险客户（D 级）
    // 用于业务层拦截：高风险客户禁止大额转账
    static bool isHighRisk(double balance, int creditScore) {
        return creditScore < 600;
    }

    // 获取风险等级数值（1=最好 AAA, 6=最差 D）
    // 用于排序比较
    static int getRiskLevel(double balance, int creditScore) {
        if (creditScore >= 800) {
            if (balance > 100000) return 1;  // AAA
            else return 2;                    // AA
        } else if (creditScore >= 700) {
            if (balance > 50000) return 3;    // A
            else return 4;                     // B
        } else if (creditScore >= 600) {
            return 5;                          // C
        } else {
            return 6;                          // D
        }
    }

    // 生成详细的风险评估报告
    static std::string getDetailedReport(double balance, int creditScore) {
        std::string report;
        report += "┌─── 风险评估报告 ───┐\n";
        report += "│ 信用等级: " + evaluate(balance, creditScore) + "\n";
        report += "│ 风险数值: " + std::to_string(getRiskLevel(balance, creditScore)) + "/6\n";
        
        if (isHighRisk(balance, creditScore)) {
            report += "│ ⚠ 警告: 该客户为高风险客户!\n";
            report += "│ ⚠ 大额转账将被限制（上限5000元）\n";
        } else if (creditScore < 700) {
            report += "│ 提示: 信用一般，建议关注\n";
        } else {
            report += "│ 状态: 信用良好\n";
        }
        
        report += "└────────────────────┘";
        return report;
    }
};

#endif // DECISION_TREE_H
