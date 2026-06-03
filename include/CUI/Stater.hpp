#pragma once

#include <IO.hpp>
#include <Types.hpp>
#include <unordered_map>

namespace mentor
{

/// 统计器
class Stater
{
public:
    Stater(IO &io) : m_io(io) {}

    void activate(const Test &t)
    {
        m_test = &t;
    }

    void markQuestionAsInvalid(int16_t qid)
    {
        m_data_map[qid].is_valid = false;
    }

    void remAnswer(int16_t qid, const std::vector<std::string> &list)
    {
        m_data_map[qid].answers = list;
    }

    void addAnswerTime(int16_t qid)
    {
        ++m_data_map[qid].time;
    }

    void markQuestion(int16_t qid, double correctness)
    {
        m_data_map[qid].correctness = correctness;
    }

    void editDiag(int16_t qid, int16_t line, const std::string &diag_p1, const std::string &symbol, const std::string &diag_p2)
    {
        m_data_map[qid].diags[line] = {diag_p1, symbol, diag_p2};
    }

    void showStatistic()
    {
        m_io.printLineCentered("测试报告");

        for (auto &q : m_test->questions)
        {
            if (!m_data_map[q.id].is_valid)
            {
                m_io.printLine(Color::Yellow, "第 {} 题异常，已跳过\n", q.id);
                continue;
            }

            m_io.printLine("第 {} 题", q.id);
            m_io.printLine("{}", q.stem);

            if (q.type == QuestionType::Choice)
                showOptions(q);

            m_io.print("\n");
            showDetails(q);
            m_io.print("\n");
        }

        double correctness{};
        if (m_test->score <= 0)
            correctness = 1.0;
        else
            correctness = static_cast<double>(m_actual_score) / m_test->score;

        if (m_test->mode == TestMode::Test)
            m_io.printLine(getScoreColor(correctness), "总分:{}/{}", m_actual_score, m_test->score);
    }

private:
    Color getScoreColor(double correctness)
    {
        if (correctness == 1.0)
            return Color::Green;

        if (correctness == 0.0)
            return Color::Red;

        return Color::Yellow;
    }

    void showOptions(const Question &q)
    {
        m_io.printLine("\n选项：");
        char choice = 'A';
        for (auto &op : q.options)
            m_io.printLine("{}. {}", choice++, op);
    }

    void showDetails(const Question &q)
    {
        if (q.type == QuestionType::Choice)
            showChoiceQuestionDetails(q);
        else if (q.type == QuestionType::Essay)
            showEssayQuestionDetails(q);
    }

    void showChoiceQuestionDetails(const Question &q)
    {
        if (m_test->mode == TestMode::Practice)
            showChoiceQuestionDetailsOnPracticeMode(q);
        else if (m_test->mode == TestMode::Test)
            showChoiceQuestionDetailsOnTestMode(q);
    }

    void showChoiceQuestionDetailsOnPracticeMode(const Question &q)
    {
        m_io.printLine(Color::Green, "答案：{}", q.answers.front());

        if (!q.solution.empty())
            m_io.printLine("解析：{}", q.solution);

        m_io.printLine("尝试次数：{}", m_data_map[q.id].time);
    }

    void showChoiceQuestionDetailsOnTestMode(const Question &q)
    {
        showScore(q);

        m_io.printLine("答案：");
        if (!m_data_map[q.id].diags.contains(0))
        {
            m_io.printLine(Color::Green, "{}", q.answers.front());
            m_io.print("\n");
        }
        else
        {
            auto diag = m_data_map[q.id].diags[0];
            if (!diag.input.empty())
                m_io.printLine("{}", diag.input);
            if (!diag.symbol.empty())
                m_io.printLine(Color::Blue, "{}", diag.symbol);
            if (!diag.expect.empty())
                m_io.printLine("{}", diag.expect);
            m_io.print("\n");
        }

        if (!q.solution.empty())
            m_io.printLine("解析：{}", q.solution);
    }

    void showEssayQuestionDetails(const Question &q)
    {
        if (m_test->mode == TestMode::Practice)
            showEssayQuestionDetailsOnPracticeMode(q);
        else if (m_test->mode == TestMode::Test)
            showEssayQuestionDetailsOnTestMode(q);
    }

    void showEssayQuestionDetailsOnPracticeMode(const Question &q)
    {
        m_io.printLine("答案：");
        for (size_t i = 0; i < q.answers.size(); ++i)
        {
            m_io.printLine("第 {} 行：", i + 1);
            m_io.printLine(Color::Green, "{}", q.answers[i]);
            m_io.print("\n");
        }

        if (!q.solution.empty())
            m_io.printLine("解析：{}", q.solution);

        m_io.printLine("尝试次数：{}", m_data_map[q.id].time);
    }

    void showEssayQuestionDetailsOnTestMode(const Question &q)
    {
        showScore(q);

        m_io.printLine("答案：");
        for (size_t i = 0; i < q.answers.size(); ++i)
        {
            m_io.printLine("第 {} 行：", i + 1);
            if (!m_data_map[q.id].diags.contains(i))
            {
                m_io.printLine(Color::Green, "{}", q.answers[i]);
                m_io.print("\n");
                continue;
            }
            auto diag = m_data_map[q.id].diags[i];
            if (!diag.input.empty())
                m_io.printLine("{}", diag.input);
            if (!diag.symbol.empty())
                m_io.printLine(Color::Blue, "{}", diag.symbol);
            if (!diag.expect.empty())
                m_io.printLine("{}", diag.expect);
            m_io.print("\n");
        }
    }

    void showScore(const Question &q)
    {
        auto correctness = m_data_map[q.id].correctness;

        int16_t score = correctness * q.score;
        m_actual_score += score;

        m_io.printLine(getScoreColor(correctness), "得分：{}", score);
    }

private:
    struct Diag
    {
        std::string input;
        std::string symbol;
        std::string expect;
    };

    struct StatData
    {
        std::unordered_map<int16_t, Diag> diags;
        std::vector<std::string> answers;
        double correctness{};
        int16_t time{};
        bool is_valid{true};
    };

private:
    IO &m_io;
    const Test *m_test{nullptr};
    std::unordered_map<int16_t, StatData> m_data_map;
    int16_t m_actual_score{};
};

} // namespace mentor