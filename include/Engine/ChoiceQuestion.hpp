#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/UI.hpp>
#include <Engine/QuestionPackage.hpp>
#include <Tool/Platform.hpp>
#include <algorithm>
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

    static QuestionPackage getEchoPackage(const Test &t, Question &q, std::string_view input, std::uint32_t &line_counter)
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

    static ContentFrame getReportFrame(const Test &t, const Question &q)
    {
        ContentFrame frame;
        auto id1 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id1).emplace_back(std::format("（{}选）第 {}/{} 题", q.answers.front().length() > 1ULL ? "多" : "单", q.id, t.questions.size()));
        frame.getComponentData(id1).emplace_back(q.stem);
        auto id2                    = frame.getComponentId(UIComponent::ChoicePanel);
        frame.getComponentData(id2) = buildNonTitleList(q.options);

        auto id3 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id3).emplace_back("答案：");
        frame.getComponentData(id3).emplace_back(q.user_inputs.front());
        frame.getComponentData(id3).emplace_back(q.diag_messages.front());

        if (!q.solution.empty())
        {
            auto id4 = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id4).emplace_back("题解：");
            frame.getComponentData(id4).emplace_back(q.solution);
        }

        frame.action     = UIAction::Clear;
        frame.input_mode = UIInputMode::QuickReact;
        return frame;
    }

private:
    static QuestionPackage getTestEchoPackage(Question &q, std::string_view input)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers.front());
        q.actual_score      = q.score * package.correctness;
        package.has_finish  = true;

        auto sorted_options = sortOptions(input);
        q.diag_messages.emplace_back(getDiag(sorted_options, q.answers.front()));
        q.user_inputs.emplace_back(sorted_options);
        return package;
    }

    static QuestionPackage getPracticeEchoPackage(Question &q, std::string_view input)
    {
        QuestionPackage package;
        package.correctness = getCorrectness(input, q.answers.front());
        package.has_finish  = true;
        auto &frame         = package.frame;
        if (package.correctness != 1.0)
        {
            auto id = frame.getComponentId(UIComponent::AnsweringBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "答案错误，请重试");
            q.user_inputs.clear();
            q.diag_messages.clear();
            return package;
        }
        auto sorted_options = sortOptions(input);
        q.diag_messages.emplace_back(getDiag(sorted_options, q.answers.front()));
        q.user_inputs.emplace_back(sorted_options);
        return package;
    }

private:
    static ContentString getDiag(std::string &input, std::string_view exp)
    {
        ContentString diag;
        size_t user{};
        size_t answer{};
        std::string temp;
        while (user < input.length() && answer < exp.length())
        {
            char uc = input[user];
            char ac = exp[answer];
            ContentChar ch;
            if (uc == ac)
            {
                temp += uc;
                ch.text = ' ';
                diag.text.emplace_back(ch);
                ++user;
                ++answer;
                continue;
            }
            if (uc < ac)
            {
                temp += uc;
                ch.text = ' ';
                ch.diag = Diag::Delete;
                ++user;
            }
            else
            {
                temp += ' ';
                ch.text = ac;
                ch.diag = Diag::Insert;
                ++answer;
            }
            diag.text.emplace_back(ch);
        }
        while (user < input.length())
        {
            ContentChar ch;
            temp += input[user];
            ch.text = ' ';
            ch.diag = Diag::Delete;
            diag.text.emplace_back(ch);
            ++user;
        }
        while (answer < exp.length())
        {
            ContentChar ch;
            temp += ' ';
            ;
            ch.text = exp[answer];
            ch.diag = Diag::Insert;
            diag.text.emplace_back(ch);
            ++answer;
        }
        input = temp;
        return diag;
    }

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

    static std::string sortOptions(std::string_view input)
    {
        std::string temp;
        for (auto ch : input)
            temp += std::toupper(ch);
        std::sort(temp.begin(), temp.end(), std::less<>());
        return temp;
    }
};

}; // namespace mentor