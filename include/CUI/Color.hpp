#pragma once

#include <Base/ContentFrame.hpp>

namespace mentor
{

constexpr std::string_view getLineColorCode(ContentStringType type)
{
    switch (type)
    {
    case ContentStringType::Success:
        return "\033[1;32m"; // 粗体绿
    case ContentStringType::Error:
        return "\033[1;31m"; // 粗体红
    case ContentStringType::Warning:
        return "\033[1;35m"; // 粗体品红
    case ContentStringType::Note:
        return "\033[1;36m"; // 粗体青
    case ContentStringType::Normal:
    default:
        return "\033[0m"; // 重置
    }
}

constexpr std::string_view getDiagColorCode(Diag diag)
{
    switch (diag)
    {
    case Diag::Insert:
        return "\033[1;32m"; // 粗体绿
    case Diag::Delete:
        return "\033[1;31m"; // 粗体红
    default:
        return "\033[0m";
    }
}

constexpr std::string_view RESET_COLOR = "\033[0m";

} // namespace mentor