#pragma once

#include <Base/UI.hpp>
#include <CUI/Color.hpp>
#include <Engine/ChoiceQuestion.hpp>
#include <Engine/EssayQuestion.hpp>
#include <Engine/Policy.hpp>
#include <Tool/Platform.hpp>
#include <format>
#include <string_view>

namespace mentor
{

/// 测试器
class Tester
{
public:
    ContentFrame getInit(Test &t)
    {
        m_test = &t;

        if (t.mode == TestMode::Unspec)
        {
            ContentFrame frame;
            frame.action                = UIAction::Clear;
            auto id1                    = frame.getComponentId(UIComponent::List);
            frame.getComponentData(id1) = buildList("选择模式", {"练习", "测试"});
            auto id2                    = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id2).emplace_back("输入序号选择");
            m_state = State::ChooseMode;
            return frame;
        }

        m_state = State::PreviewPause;
        return getPreviewFrame();
    }

    ContentFrame feedInput(std::string_view input)
    {
        switch (m_state)
        {
        case State::ChooseMode:
            return handleChooseMode(input);
        case State::PreviewPause:
            return handlePreviewPause(input);
        case State::AnsweringPause:
            return handleAnsweringPause(input);
        case State::Answering:
            return handleAnswering(input);
        default:
            unreachable();
        }
    }

public:
    bool hasFinish() const noexcept
    {
        return m_state == State::Finish;
    }

private:
    ContentFrame handleChooseMode(std::string_view input)
    {
        ContentFrame frame;

        std::int32_t index;
        auto result = std::from_chars(input.data(), input.data() + input.size(), index);

        if (result.ec == std::errc())
        {
            if (index > 2)
            {
                auto id = frame.getComponentId(UIComponent::InputBox);
                frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入编号大于最大编号，请重新输入");
                return frame;
            }
            else if (index <= 0)
            {
                auto id = frame.getComponentId(UIComponent::InputBox);
                frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入编号小于最小编号，请重新输入");
                return frame;
            }
        }
        else if (result.ec == std::errc::invalid_argument)
        {
            auto id = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入不是一个数字，请重新输入");
            return frame;
        }
        else
        {
            auto id = frame.getComponentId(UIComponent::InputBox);
            frame.getComponentData(id).emplace_back(ContentStringType::Error, "输入异常，请重新输入");
            return frame;
        }

        m_state      = State::PreviewPause;
        m_test->mode = static_cast<TestMode>(index);
        return getPreviewFrame();
    }

    ContentFrame handlePreviewPause(std::string_view input)
    {
        m_state = State::Answering;
        return getQuestionFrame();
    }

    ContentFrame handleAnswering(std::string_view input)
    {
        return getQuestionEchoFrame(input);
    }

    ContentFrame handleAnsweringPause(std::string_view input)
    {
        m_state = State::Answering;
        return getQuestionFrame();
    }

private:
    ContentFrame getPreviewFrame()
    {
        ContentFrame frame;
        frame.action = UIAction::Clear;
        auto id      = frame.getComponentId(UIComponent::PreviewPanel);
        if (m_test->mode == TestMode::Practice)
        {
            frame.getComponentData(id).emplace_back(std::format("练习：{}", m_test->name));
            frame.getComponentData(id).emplace_back(std::format("题数：{}", m_test->questions.size()));
        }
        else if (m_test->mode == TestMode::Test)
        {
            initTestScore();
            frame.getComponentData(id).emplace_back(std::format("测试：{}", m_test->name));
            frame.getComponentData(id).emplace_back(std::format("题数：{}", m_test->questions.size()));
            frame.getComponentData(id).emplace_back(std::format("总分：{}", m_test->score));
        }
        frame.getComponentData(id).emplace_back();
        frame.getComponentData(id).emplace_back(ContentStringType::Success, "按任意键继续");
        frame.input_mode = UIInputMode::QuickReact;
        return frame;
    }

    ContentFrame getQuestionFrame()
    {
        auto &q = m_test->questions[m_index];

        if (q.answers.empty() || q.answers.front().length() == 0ULL)
        {
            ++m_index;
            ContentFrame frame;
            frame.action = UIAction::Clear;
            auto id      = frame.getComponentId(UIComponent::Text);
            frame.getComponentData(id).emplace_back(ContentStringType::Warning, std::format("警告：已跳过第 {} 题", q.id));
            frame.getComponentData(id).emplace_back(ContentStringType::Warning, "答案为空");
            frame.getComponentData(id).emplace_back();
            frame.getComponentData(id).emplace_back("按任意键继续");
            frame.input_mode = UIInputMode::QuickReact;
            m_state          = State::AnsweringPause;
            return frame;
        }

        switch (q.type)
        {
        case QuestionType::Choice:
            return ChoiceQuestion::getInitFrame(*m_test, q);
        case QuestionType::Essay:
            return EssayQuestion::getInitFrame(*m_test, q);
        default:
            unreachable();
        }
    }

    ContentFrame getQuestionEchoFrame(std::string_view input)
    {
        auto &q = m_test->questions[m_index];

        QuestionPackage package{};

        switch (q.type)
        {
        case QuestionType::Choice:
            package = ChoiceQuestion::getEchoPackage(*m_test, q, input);
            break;
        case QuestionType::Essay:
            package = EssayQuestion::getEchoPackage(*m_test, q, input, m_line_counter);
            break;
        }

        auto frame = Policy::actOnQuestionPackage(*m_test, m_index, m_line_counter, package);
        if (m_index == m_test->questions.size())
            m_state = State::Finish;
        return frame;
    }

private:
    /// 初始化测试分数
    void initTestScore()
    {
        if (m_test->score != 0)
            autoDistributeScore();
        else
            autoSummarizeScore();
    }

    /// 自动分配每题的分数
    void autoDistributeScore()
    {
        uint16_t total_score = m_test->score;

        uint16_t average_score = total_score / m_test->questions.size();
        uint16_t last_score    = total_score - average_score * (m_test->questions.size() - 1);

        for (size_t i = 0; i < m_test->questions.size() - 1; ++i)
            m_test->questions[i].score = average_score;

        m_test->questions.back().score = last_score;
    }

    /// 自动求总分
    void autoSummarizeScore()
    {
        uint16_t total_score{};

        for (const auto &q : m_test->questions)
            total_score += q.score;

        m_test->score = total_score;
    }

private:
    enum class State
    {
        Init,
        ChooseMode,
        PreviewPause,
        Answering,
        AnsweringPause,
        Finish
    } m_state = State::Init;
    Test *m_test{nullptr};
    std::size_t m_index{};
    std::uint32_t m_line_counter{1};
};

} // namespace mentor