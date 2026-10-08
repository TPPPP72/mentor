#include "Codec/Model.hpp"
#include "Base/Model.hpp"

namespace mentor
{

std::optional<TestInfo> decodeTestInfo(const nlohmann::json &j)
{
    if (!j.contains("id"))
        return std::nullopt;

    TestInfo info;
    info.id          = j["id"].get<uint32_t>();
    info.name        = j.value("name", "");
    info.author      = j.value("author", "");
    info.description = j.value("description", "");
    info.score       = j.value("score", 100u);
    info.mode        = j.value("mode", TestMode::unspecified);
    return info;
}

std::optional<ChoiceQuestion> decodeChoiceQuestion(const nlohmann::json &j)
{
    if (!j.contains("options") || !j.contains("answer"))
        return std::nullopt;

    ChoiceQuestion question;
    question.id       = j["id"].get<uint32_t>();
    question.stem     = j["stem"].get<std::string>();
    question.options  = j["options"].get<std::vector<std::string>>();
    question.answer   = j["answer"].get<uint32_t>();
    question.hint     = j.value("hint", "");
    question.solution = j.value("solution", "");
    question.score    = j.value("score", 0u);
    return question;
}

std::optional<EssayQuestion> decodeEssayQuestion(const nlohmann::json &j)
{
    if (!j.contains("answers"))
        return std::nullopt;

    EssayQuestion question;
    question.id       = j["id"].get<uint32_t>();
    question.stem     = j["stem"].get<std::string>();
    question.answers  = j["answers"].get<std::vector<std::string>>();
    question.hint     = j.value("hint", "");
    question.solution = j.value("solution", "");
    question.score    = j.value("score", 0u);
    return question;
}

} // namespace mentor
