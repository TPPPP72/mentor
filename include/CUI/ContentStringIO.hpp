#pragma once

#include <CUI/Color.hpp>
#include <CUI/Util.hpp>
#include <Base/ContentFrame.hpp>
#include <format>

namespace mentor
{

inline std::string formatContentStringInternal(const ContentString &str)
{
    std::string result;

    if (str.type != ContentStringType::Normal)
        result += getLineColorCode(str.type);

    for (auto ch : str.text)
        result += ch.text;

    result += RESET_COLOR;

    bool hasDiag = false;
    std::string diagLine;

    for (auto ch : str.text)
    {
        int width = getCharDisplayWidth(ch.text);
        if (width == 0)
            continue;

        if (ch.diag == Diag::Insert)
        {
            diagLine += std::format("{}+{:<{}}", getDiagColorCode(Diag::Insert), "", width - 1);
            hasDiag = true;
        }
        else if (ch.diag == Diag::Delete)
        {
            diagLine += std::format("{}-{:<{}}", getDiagColorCode(Diag::Delete), "", width - 1);
            hasDiag = true;
        }
        else
        {
            diagLine += std::format("{:<{}}", "", width);
        }
    }

    if (hasDiag)
    {
        result += '\n';
        result += diagLine;
        result += RESET_COLOR;
    }

    return result;
}

inline std::ostream &operator<<(std::ostream &os, const mentor::ContentString &str)
{
    return os << formatContentStringInternal(str);
}

} // namespace mentor

template <>
struct std::formatter<mentor::ContentString> : std::formatter<std::string>
{
    auto format(const mentor::ContentString &str, format_context &ctx) const
    {
        return std::formatter<std::string>::format(mentor::formatContentStringInternal(str), ctx);
    }
};