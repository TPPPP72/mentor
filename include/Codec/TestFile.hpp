#pragma once

#include "Codec/Model.hpp"
#include "File/TestFile.hpp"

namespace mentor::codec
{

inline std::optional<file::TestFile> decodeTestFile(const nlohmann::json &j)
{
    if (!j.contains("questions"))
        return std::nullopt;

    auto ti = codec::decode<TestInfo>(j);
    if (!ti.has_value())
        return std::nullopt;

    file::TestFile tf;
    tf.info      = ti.value();
    tf.questions = j["questions"];
    return tf;
}

template <typename File>
std::optional<File> decode(const nlohmann::json &j)
    requires std::same_as<File, file::TestFile>
{
    return decodeTestFile(j);
}

} // namespace mentor
