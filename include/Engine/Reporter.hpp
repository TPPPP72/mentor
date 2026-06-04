#pragma once

#include <Base/UI.hpp>
#include <Engine/ChoiceQuestion.hpp>
#include <Engine/EssayQuestion.hpp>

namespace mentor
{

class Reporter
{
public:
    ContentFrame getInit(Test &t)
    {
        m_test = &t;
        switch (t.mode)
        {
        case TestMode::Test:
            return getOverview();
        case TestMode::Practice:
            ++m_index;
            return getQuestion(m_test->questions.front());
        default:
            unreachable();
        }
    }

    ContentFrame feedInput(std::string_view input)
    {
        if ((input == "A" || input == "a") && m_index > 0)
            --m_index;
        else if ((input == "D" || input == "d") && m_index < m_test->questions.size() - 1)
            ++m_index;
        return getQuestion(m_test->questions[m_index]);
    }

private:
    ContentFrame getOverview()
    {
        ContentFrame frame;

        std::uint32_t score{};
        for (auto &q : m_test->questions)
            score += q.actual_score;

        auto id1 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id1).emplace_back(std::format("得分：{} / {}", score, m_test->score));

        auto id2 = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id2).emplace_back("使用 A / D 预览报告");
        frame.action     = UIAction::Clear;
        frame.input_mode = UIInputMode::QuickReact;
        return frame;
    }

    ContentFrame getQuestion(const Question &q)
    {
        switch (q.type)
        {
        case QuestionType::Choice:
            return ChoiceQuestion::getReportFrame(*m_test, q);
        case QuestionType::Essay:
            return EssayQuestion::getReportFrame(*m_test, q);
        default:
            unreachable();
        }
    }

private:
    Test *m_test{nullptr};
    std::size_t m_index{};
};

} // namespace mentor