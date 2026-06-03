#pragma once

#include <fstream>
#include <Base/Type.hpp>

namespace mentor{
    void readTestFromJson(std::ifstream &, Test &);
}