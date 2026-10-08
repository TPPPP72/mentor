#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace mentor
{

enum class TestMode : uint32_t
{
    unspecified,
    practice,
    test
};

struct TestInfo
{
    std::string name;
    std::string author;
    std::string description;
    TestMode mode;
    uint32_t id;
    uint32_t score;
};

enum class QuestionType : uint32_t
{
    choice,
    essay
};

struct ChoiceQuestion
{
    std::string stem;
    std::string hint;
    std::string solution;
    std::vector<std::string> options;
    uint32_t answer;
    uint32_t id;
    uint32_t score;
};

struct EssayQuestion
{
    std::string stem;
    std::string hint;
    std::string solution;
    std::vector<std::string> answers;
    uint32_t id;
    uint32_t score;
};

} // namespace mentor
