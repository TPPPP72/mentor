#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/Type.hpp>
#include <Engine/QuestionPackage.hpp>
#include <Tool/Platform.hpp>
#include <format>

namespace mentor
{

/// 简答题
class EssayQuestion
{
public:
    static ContentFrame getInitFrame(const Test &t, const Question &q)
    {
        ContentFrame frame;
        auto id1 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id1).emplace_back(std::format("（简答）第 {}/{} 题", q.id, t.questions.size()));
        frame.getComponentData(id1).emplace_back(q.stem);

        if (!q.hint.empty())
        {
            auto id2 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id2).emplace_back(ContentStringType::Note, std::format("提示：{}", q.hint));
        }

        auto id4 = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id4).emplace_back("第 1 行");

        frame.action = UIAction::Clear;
        return frame;
    }

    static QuestionPackage getEchoPackage(const Test &t, const Question &q, std::string_view input, std::uint32_t &line)
    {
        switch (t.mode)
        {
        case TestMode::Practice:
            return getPracticeEchoPackage(q, input, line);
        case TestMode::Test:
            return getTestEchoPackage(q, input, line);
        default:
            unreachable();
        }
    }

private:
    static QuestionPackage getTestEchoPackage(const Question &q, std::string_view input, std::uint32_t &line)
    {
        QuestionPackage package;
        package.correctness += getCorrectness(input, q.answers[line - 1]);
        if (line == q.answers.size())
        {
            package.correctness /= q.answers.size();
            package.has_finish = true;
            return package;
        }
        ++line;
        auto &frame = package.frame;
        auto id     = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id).emplace_back(std::format("第 {} 行", line));
        return package;
    }

    static QuestionPackage getPracticeEchoPackage(const Question &q, std::string_view input, std::uint32_t &line)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers[line - 1]);
        auto &frame         = package.frame;
        if (package.correctness != 1.0)
        {
            auto id1 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id1).emplace_back(ContentStringType::Error, "答案错误，请重试");
            auto id2 = frame.getComponentId(UIComponent::AnsweringBox);
            frame.getComponentData(id2).emplace_back("第 1 行");
            return package;
        }
        if (line == q.answers.size())
        {
            package.has_finish = true;
            return package;
        }
        ++line;
        auto id     = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id).emplace_back(std::format("第 {} 行", line));
        return package;
    }

    static double getCorrectness(std::string_view input, std::string_view answer)
    {
        if (input.length() < answer.length())
            return 0.0;

        for (size_t i = 0; i < answer.length(); ++i)
        {
            if (input[i] != answer[i])
                return 0.0;
        }

        return 1.0;
    }

    static bool compareAndDiag(std::string_view user_input, std::string_view expect_answer, int16_t qid, int16_t line)
    {
        size_t n = user_input.length();
        size_t m = expect_answer.length();

        std::vector<std::vector<int32_t>> dp(n + 1, std::vector<int32_t>(m + 1));
        for (size_t i = 0; i <= n; ++i)
            dp[i][0] = i;
        for (size_t j = 0; j <= m; ++j)
            dp[0][j] = j;

        for (size_t i = 1; i <= n; ++i)
        {
            for (size_t j = 1; j <= m; ++j)
            {
                if (user_input[i - 1] == expect_answer[j - 1])
                    dp[i][j] = dp[i - 1][j - 1];
                else
                    dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }

        std::string diag_p1, symbol, diag_p2;
        size_t i = n, j = m;
        while (i > 0 || j > 0)
        {
            if (i > 0 && j > 0 && user_input[i - 1] == expect_answer[j - 1])
            {
                diag_p1 += user_input[i - 1];
                diag_p2 += ' ';
                symbol += ' ';
                i--;
                j--;
            }
            else if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + 1)
            {
                diag_p1 += user_input[i - 1];
                diag_p2 += expect_answer[j - 1];
                symbol += '^';
                i--;
                j--;
            }
            else if (i > 0 && (j == 0 || dp[i][j] == dp[i - 1][j] + 1))
            {
                diag_p1 += user_input[i - 1];
                diag_p2 += ' ';
                symbol += '~';
                i--;
            }
            else
            {
                diag_p1 += ' ';
                diag_p2 += expect_answer[j - 1];
                symbol += '^';
                j--;
            }
        }

        std::reverse(diag_p1.begin(), diag_p1.end());
        std::reverse(symbol.begin(), symbol.end());
        std::reverse(diag_p2.begin(), diag_p2.end());

        // m_stator.editDiag(qid, line, diag_p1, symbol, diag_p2);

        return n == m && dp[n][m] == 0;
    }
};

} // namespace mentor