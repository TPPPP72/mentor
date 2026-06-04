#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/Type.hpp>
#include <Engine/QuestionPackage.hpp>
#include <Tool/Platform.hpp>
#include <format>
#include <string_view>

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

    static QuestionPackage getEchoPackage(const Test &t, Question &q, std::string_view input, std::uint32_t &line)
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

    static ContentFrame getReportFrame(const Test &t, const Question &q)
    {
        ContentFrame frame;
        auto id1 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id1).emplace_back(std::format("（简答）第 {}/{} 题", q.id, t.questions.size()));
        frame.getComponentData(id1).emplace_back(q.stem);

        auto id2 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id2).emplace_back("诊断：");
        for (size_t i = 0; i < q.user_inputs.size(); ++i)
        {
            frame.getComponentData(id2).emplace_back(std::format("第 {} 行：", i+1));
            frame.getComponentData(id2).emplace_back(q.user_inputs[i]);
            frame.getComponentData(id2).emplace_back(q.diag_messages[i]);
        }

        if (!q.solution.empty())
        {
            auto id3 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id3).emplace_back("题解：");
            frame.getComponentData(id3).emplace_back(q.solution);
        }

        frame.action     = UIAction::Clear;
        frame.input_mode = UIInputMode::QuickReact;
        return frame;
    }

private:
    static QuestionPackage getTestEchoPackage(Question &q, std::string_view input, std::uint32_t &line)
    {
        QuestionPackage package;
        q.user_inputs.emplace_back(input);
        q.diag_messages.emplace_back(getDiag(input, q.answers[line - 1]));
        package.correctness += getCorrectness(input, q.answers[line - 1]);
        if (line == q.answers.size())
        {
            package.correctness /= q.answers.size();
            package.has_finish = true;
            q.actual_score = package.correctness * q.score;
            return package;
        }
        ++line;
        auto &frame = package.frame;
        auto id     = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id).emplace_back(std::format("第 {} 行", line));
        return package;
    }

    static QuestionPackage getPracticeEchoPackage(Question &q, std::string_view input, std::uint32_t &line)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers[line - 1]);
        q.user_inputs.emplace_back(input);
        q.diag_messages.emplace_back(getDiag(input, q.answers[line - 1]));
        auto &frame = package.frame;
        if (package.correctness != 1.0)
        {
            auto id1 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id1).emplace_back(ContentStringType::Error, "答案错误，请重试");
            q.user_inputs.clear();
            q.diag_messages.clear();
            line     = 1;
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
        auto id = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id).emplace_back(std::format("第 {} 行", line));
        return package;
    }

private:
    static ContentString getDiag(std::string_view input, std::string_view answer)
    {
        ContentString diag;
        size_t n = input.length();
        size_t m = answer.length();

        std::vector<std::vector<int32_t>> dp(n + 1, std::vector<int32_t>(m + 1));
        for (size_t i = 0; i <= n; ++i)
            dp[i][0] = i;
        for (size_t j = 0; j <= m; ++j)
            dp[0][j] = j;

        for (size_t i = 1; i <= n; ++i)
        {
            for (size_t j = 1; j <= m; ++j)
            {
                if (input[i - 1] == answer[j - 1])
                    dp[i][j] = dp[i - 1][j - 1];
                else
                    dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }

        size_t i = n, j = m;
        while (i > 0 || j > 0)
        {
            if (i > 0 && j > 0 && input[i - 1] == answer[j - 1])
            {
                ContentChar ch;
                ch.text = ' ';
                ch.diag = Diag::Null;
                diag.text.emplace_back(ch);
                i--;
                j--;
            }
            else if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + 1)
            {
                ContentChar ch;
                ch.text = answer[j - 1];
                ch.diag = Diag::Insert;
                diag.text.emplace_back(ch);
                i--;
                j--;
            }
            else if (i > 0 && (j == 0 || dp[i][j] == dp[i - 1][j] + 1))
            {
                ContentChar ch;
                ch.text = ' ';
                ch.diag = Diag::Delete;
                diag.text.emplace_back(ch);
                i--;
            }
            else
            {
                ContentChar ch;
                ch.text = answer[j - 1];
                ch.diag = Diag::Insert;
                diag.text.emplace_back(ch);
                j--;
            }
        }

        for (size_t i = 0; i < diag.text.size() / 2; ++i)
            std::swap(diag.text[i], diag.text[diag.text.size() - i - 1]);

        return diag;
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
};

} // namespace mentor