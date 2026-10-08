#pragma once

#include "Base/Model.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <type_traits>

namespace mentor
{

std::optional<TestInfo> decodeTestInfo(const nlohmann::json &j);
std::optional<ChoiceQuestion> decodeChoiceQuestion(const nlohmann::json &j);
std::optional<EssayQuestion> decodeEssayQuestion(const nlohmann::json &j);

template <typename Model>
std::optional<Model> decode(const nlohmann::json &j)
{
    if constexpr (std::is_same_v<TestInfo, Model>)
    {
        return decodeTestInfo(j);
    }
    else
    {
        // every question should at least have id and stem
        if (!j.contains("id") || !j.contains("stem"))
            return std::nullopt;

        if constexpr (std::is_same_v<ChoiceQuestion, Model>)
            return decodeChoiceQuestion(j);
        else
            return decodeEssayQuestion(j);
    }
}

} // namespace mentor
