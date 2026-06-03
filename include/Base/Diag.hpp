#pragma once

#include <cstdint>

namespace mentor
{

enum class Diag : uint8_t
{
    Null,   ///< 无附加诊断
    Insert, ///< 插入诊断
    Delete, ///< 删除诊断
};

} // namespace mentor
