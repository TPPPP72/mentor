#pragma once

#include "Codec/Model.hpp"
#include "File/TestFile.hpp"

namespace mentor
{

inline std::optional<TestFile> decodeTestFile(const nlohmann::json &j)
{
    if (!j.contains("questions"))
        return std::nullopt;

    auto ti = decode<TestInfo>(j);
    if (!ti.has_value())
        return std::nullopt;

    TestFile tf;
    tf.info      = ti.value();
    tf.questions = j["questions"];
    return tf;
}

} // namespace mentor
