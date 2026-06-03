#pragma once

#include <cstdint>
#include <Base/UI.hpp>
#include <string>

namespace mentor{

void initTerminal();
int32_t getTerminalWidth();
std::string getTerminalInput(UIInputMode mode);

inline int getCharDisplayWidth(char ch)
{
    unsigned char u_ch = static_cast<unsigned char>(ch);
    if (u_ch <= 0x7F)
        return 1;
    if ((u_ch & 0xC0) == 0x80)
        return 0;
    if ((u_ch & 0xF0) == 0xE0)
        return 2;
    return 1;
}

}