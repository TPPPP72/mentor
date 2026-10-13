#pragma once

#include "Codec/Model.hpp"
#include "Kernel/File/TestFile.hpp"

namespace mentor::codec
{

inline std::optional<kernel::TestFile> decodeTestFile(const nlohmann::json &j)
{
    if (!j.contains("questions"))
        return std::nullopt;

    auto ti = codec::decode<TestInfo>(j);
    if (!ti.has_value())
        return std::nullopt;

    kernel::TestFile tf;
    tf.info      = ti.value();
    tf.questions = j["questions"];
    return tf;
}

template <typename File>
concept IsTestFile = std::same_as<File, kernel::TestFile>;

template <typename File>
std::optional<File> decode(const nlohmann::json &j)
    requires IsTestFile<File>
{
    return decodeTestFile(j);
}

} // namespace mentor::codec
