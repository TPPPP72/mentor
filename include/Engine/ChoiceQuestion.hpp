#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/UI.hpp>
#include <Engine/QuestionPackage.hpp>
#include <Tool/Platform.hpp>
#include <bit>
#include <cctype>
#include <format>
#include <string_view>


namespace mentor
{

/// 选择题
class ChoiceQuestion
{
public:
    static ContentFrame getInitFrame(const Test &t, const Question &q)
    {
        ContentFrame frame;
        auto id1 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id1).emplace_back(std::format("（{}选）第 {}/{} 题", q.answers.front().length() > 1ULL ? "多" : "单", q.id, t.questions.size()));
        frame.getComponentData(id1).emplace_back(q.stem);
        auto id2                    = frame.getComponentId(UIComponent::ChoicePanel);
        frame.getComponentData(id2) = buildNonTitleList(q.options);

        if (!q.hint.empty())
        {
            auto id3 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id3).emplace_back(ContentStringType::Note, std::format("提示：{}", q.hint));
        }

        auto id4 = frame.getComponentId(UIComponent::AnsweringBox);
        frame.getComponentData(id4).emplace_back("答案");

        frame.action = UIAction::Clear;
        return frame;
    }

    static QuestionPackage getEchoPackage(const Test &t, const Question &q, std::string_view input)
    {
        switch (t.mode)
        {
        case TestMode::Practice:
            return getPracticeEchoPackage(q, input);
        case TestMode::Test:
            return getTestEchoPackage(q, input);
        default:
            unreachable();
        }
    }

private:
    static QuestionPackage getTestEchoPackage(const Question &q, std::string_view input)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers.front());
        package.has_finish  = true;
        return package;
    }

    static QuestionPackage getPracticeEchoPackage(const Question &q, std::string_view input)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers.front());
        package.has_finish  = true;
        auto &frame         = package.frame;
        if (package.correctness != 1.0)
        {
            auto id = frame.getComponentId(UIComponent::AnsweringBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "答案错误，请重试");
            return package;
        }
        return package;
    }

    // static double compareAndDiag(std::string_view user_input, std::string_view expect_answer, int16_t qid)
    // {
    //     int16_t incorrect_count{};
    //     size_t user{};
    //     size_t answer{};
    //     std::string diag_p1{};
    //     std::string diag_p2{};
    //     std::string symbol{};
    //     while (user < user_input.length() && answer < expect_answer.length())
    //     {
    //         char uc = user_input[user];
    //         char ac = expect_answer[answer];
    //         if (uc == ac)
    //         {
    //             diag_p1 += uc;
    //             diag_p2 += ' ';
    //             symbol += ' ';
    //             ++user;
    //             ++answer;
    //             continue;
    //         }
    //         if (uc < ac)
    //         {
    //             diag_p1 += uc;
    //             diag_p2 += ' ';
    //             symbol += '~';
    //             incorrect_count = expect_answer.length();
    //             ++user;
    //         }
    //         else
    //         {
    //             diag_p1 += ' ';
    //             diag_p2 += ac;
    //             symbol += '^';
    //             ++incorrect_count;
    //             ++answer;
    //         }
    //     }
    //     while (user < user_input.length())
    //     {
    //         char uc = user_input[user];
    //         diag_p1 += uc;
    //         symbol += '~';
    //         incorrect_count = expect_answer.length();
    //         ++user;
    //     }
    //     while (answer < expect_answer.length())
    //     {
    //         char ac = expect_answer[answer];
    //         diag_p2 += ac;
    //         symbol += '^';
    //         ++incorrect_count;
    //         ++answer;
    //     }

    //     if (incorrect_count)
    //         // m_stator.editDiag(qid, 0, diag_p1, symbol, diag_p2);

    //         if (expect_answer.length() == 0ULL)
    //         {
    //             if (user_input.length() == 0ULL)
    //                 return 1.0;
    //             else
    //                 return 0.0;
    //         }

    //     if (incorrect_count >= expect_answer.length())
    //         return 0.0;

    //     return static_cast<double>(expect_answer.length() - incorrect_count) / expect_answer.length();
    // }

private:
    static double getCorrectness(std::string_view input, std::string_view answer)
    {
        auto a = toUint32(input);
        auto b = toUint32(answer);
        if (a & ~b)
            return 0.0;

        auto correct_count = std::popcount(a & b);
        auto total_count   = std::popcount(b);
        return static_cast<double>(correct_count) / total_count;
    }

    static std::uint32_t toUint32(std::string_view input)
    {
        std::uint32_t result{};
        for (auto ch : input)
        {
            if (std::islower(ch))
                result |= (1 << (ch - 'a'));
            else if (std::isupper(ch))
                result |= (1 << (ch - 'A'));
        }
        return result;
    }
};
}; // namespace mentor