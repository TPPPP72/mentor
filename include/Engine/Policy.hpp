#pragma once

#include <Engine/Tester.hpp>
#include <Tool/Platform.hpp>

namespace mentor
{

class Policy
{
public:
    static void actOnQuestionPackage(const Test &test, size_t &current_index, std::uint32_t &line_counter, QuestionPackage &qp)
    {
        if (!qp.has_finish)
            return;

        if (current_index == test.questions.size() - 1)
        {
            ++current_index;
            qp.frame = getFinishFrame();
            return;
        }

        line_counter = 1;

        switch (test.mode)
        {
        case TestMode::Test:
            actOnTestQuestionPackage(test, current_index, qp);
            break;
        case TestMode::Practice:
            actOnPracticeQuestionPackage(test, current_index, qp);
            break;
        default:
            unreachable();
        }
    }

private:
    static void actOnTestQuestionPackage(const Test &test, size_t &current_index, QuestionPackage &qp)
    {
        ++current_index;
        qp.frame = getInitFrame(test, current_index);
    }

    static void actOnPracticeQuestionPackage(const Test &test, size_t &current_index, QuestionPackage &qp)
    {
        if (qp.correctness != 1.0)
            return;

        ++current_index;
        qp.frame = getInitFrame(test, current_index);
    }

private:
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