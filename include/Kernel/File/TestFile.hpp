#pragma once

#include "Base/Model.hpp"
#include <nlohmann/json.hpp>

namespace mentor::kernel
{

struct TestFile
{
    TestInfo info;
    nlohmann::json::array_t questions;
};

} // namespace mentor::kernel
