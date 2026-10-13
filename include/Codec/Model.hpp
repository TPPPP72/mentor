#pragma once

#include "Base/Model.hpp"
#include <concepts>
#include <nlohmann/json.hpp>
#include <optional>


namespace mentor::codec
{

std::optional<TestInfo> decodeTestInfo(const nlohmann::json &j);
std::optional<ChoiceQuestion> decodeChoiceQuestion(const nlohmann::json &j);
std::optional<EssayQuestion> decodeEssayQuestion(const nlohmann::json &j);

template <typename Info>
concept IsTestInfo = std::same_as<Info, TestInfo>;

template <typename Question>
concept IsQuestion = std::same_as<Question, ChoiceQuestion> ||
                     std::same_as<Question, EssayQuestion>;

template <typename Info>
std::optional<Info> decode(const nlohmann::json &j)
    requires IsTestInfo<Info>
{
    return decodeTestInfo(j);
}

template <typename Question>
std::optional<Question> decode(const nlohmann::json &j)
    requires IsQuestion<Question>
{
    // every question should at least have id and stem
    if (!j.contains("id") || !j.contains("stem"))
        return std::nullopt;

    if constexpr (std::is_same_v<ChoiceQuestion, Question>)
        return decodeChoiceQuestion(j);
    else
        return decodeEssayQuestion(j);
}

} // namespace mentor::codec
