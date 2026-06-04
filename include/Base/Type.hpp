#pragma once

#include <Base/ContentFrame.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace mentor
{

/// 问题类型
enum class QuestionType : uint32_t
{
    Choice, ///< 选择题
    Essay   ///< 简答题
};

/// 问题
struct Question
{
    std::string stem;                         ///< 题面
    std::string hint;                         ///< 提示（答题前显示，总是可见）
    std::string solution;                     ///< 题解（答题后显示，练习可见）
    std::vector<std::string> options;         ///< 选项
    std::vector<std::string> answers;         ///< 答案
    std::vector<std::string> user_inputs;     ///< 用户输入
    std::vector<ContentString> diag_messages; ///< 诊断信息
    QuestionType type;                        ///< 类型
    uint32_t id;                              ///< 编号
    uint32_t score;                           ///< 分数
    uint32_t actual_score;                    ///< 实际分数
};

/// 测试类型
enum class TestMode : uint32_t
{
    Unspec,   ///< 未指定
    Practice, ///< 练习
    Test      ///< 测试
};

/// 测试
struct Test
{
    std::vector<Question> questions; ///< 问题
    std::string name;                ///< 名称
    TestMode mode;                   ///< 模式
    uint32_t id;                     ///< 编号
    uint32_t score;                  ///< 总分
};

} // namespace mentor