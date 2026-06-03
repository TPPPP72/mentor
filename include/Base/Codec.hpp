#pragma once

#include <Base/Type.hpp>
#include <nlohmann/json_fwd.hpp>

namespace mentor
{

using json = nlohmann::json;

void to_json(json &j, const Question &q);

void from_json(const json &j, Question &q);

void to_json(json &j, const Test &t);

void from_json(const json &j, Test &t);

} // namespace mentor