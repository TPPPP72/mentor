#pragma once

#include "Base/ContentFrame.hpp"
#include "Base/UI.hpp"
#include <Engine/Tester.hpp>
#include <Tool/Platform.hpp>

namespace mentor
{

class Policy
{
public:
    static ContentFrame actOnQuestionPackage(const Test &test, size_t &current_index, std::uint32_t &line_counter, QuestionPackage &qp)
    {
        if (!qp.has_finish)
            return qp.frame;

        if (current_index == test.questions.size() - 1)
        {
            ++current_index;
            return getFinishFrame();
        }

        line_counter = 1;

        switch (test.mode)
        {
        case TestMode::Test:
            return actOnTestQuestionPackage(test, current_index, line_counter, qp);
        case TestMode::Practice:
            return actOnPracticeQuestionPackage(test, current_index, line_counter, qp);
        default:
            unreachable();
        }
    }

private:
    static ContentFrame actOnTestQuestionPackage(const Test &test, size_t &current_index, std::uint32_t &line_counter, QuestionPackage &qp)
    {
        ++current_index;
        qp.frame = getInitFrame(test, current_index);
        return qp.frame;
    }

    static ContentFrame actOnPracticeQuestionPackage(const Test &test, size_t &current_index, std::uint32_t &line_counter, QuestionPackage &qp)
    {
        if (qp.correctness != 1.0)
            return qp.frame;
 
        ++current_index;
        qp.frame = getInitFrame(test, current_index);
        return qp.frame;
    }

    static ContentFrame getInitFrame(const Test &test, size_t &current_index)
    {
        auto &q = test.questions[current_index];
        switch (q.type)
        {
        case QuestionType::Choice:
            return ChoiceQuestion::getInitFrame(test, q);
        case QuestionType::Essay:
            return EssayQuestion::getInitFrame(test, q);
        default:
            unreachable();
        }
    }

    static ContentFrame getFinishFrame()
    {
        ContentFrame frame;
        auto id = frame.getComponentId(UIComponent::Text);
        frame.getComponentData(id).emplace_back(ContentStringType::Success, "测试已完成，按任意键查看报告");
        frame.action     = UIAction::Clear;
        frame.input_mode = UIInputMode::QuickReact;
        return frame;
    }
};

} // namespace mentor