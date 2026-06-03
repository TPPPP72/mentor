#pragma once

#include <Base/ContentFrame.hpp>
#include <Base/Type.hpp>

namespace mentor
{

struct QuestionPackage
{
    ContentFrame frame;
    double correctness{};
    bool has_finish{};
};

} // namespace mentor