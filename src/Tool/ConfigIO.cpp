#include <Base/Codec.hpp>
#include <Tool/ConfigIO.hpp>
#include <nlohmann/json.hpp>

void mentor::readTestFromJson(std::ifstream &i, Test &t)
{
    nlohmann::json j;
    i >> j;
    t = j;
}