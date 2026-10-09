#pragma once

#include "Base/Model.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <type_traits>

namespace mentor::codec
{

std::optional<TestInfo> decodeTestInfo(const nlohmann::json &j);
std::optional<ChoiceQuestion> decodeChoiceQuestion(const nlohmann::json &j);
std::optional<EssayQuestion> decodeEssayQuestion(const nlohmann::json &j);

template <typename Model>
concept IsModel = std::same_as<Model, TestInfo> ||
                  std::same_as<Model, ChoiceQuestion> ||
                  std::same_as<Model, EssayQuestion>;

template <typename Model>
std::optional<Model> decode(const nlohmann::json &j) requires IsModel<Model>
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
