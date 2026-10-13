#pragma once

#include "Codec/TestFile.hpp"
#include "Kernel/File/TestFile.hpp"
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <vector>
#include <sstream>

namespace mentor::kernel
{

inline std::optional<std::vector<std::filesystem::path>> getDirFiles(const std::filesystem::path &p,
                                                                     std::function<bool(const std::filesystem::path &p)> filter = nullptr,
                                                                     bool is_recursive                                          = true)
{
    if (!std::filesystem::is_directory(p))
        return std::nullopt;

    std::vector<std::filesystem::path> files;

    auto collect = [&files, filter]<typename Iter>(Iter it)
    {
        for (const auto &entry : it)
        {
            if (entry.is_regular_file())
            {
                if (!filter || filter(entry))
                    files.push_back(entry);
            }
        }
    };

    if (is_recursive)
        collect(std::filesystem::recursive_directory_iterator(p));
    else
        collect(std::filesystem::directory_iterator(p));

    return files;
}

inline std::optional<std::vector<std::filesystem::path>> getCurrentDirFiles()
{
    return getDirFiles(std::filesystem::current_path());
}

inline std::optional<TestFile> readTestFile(const std::filesystem::path &p)
{
    std::ifstream i(p);
    if (!i.is_open())
        return std::nullopt;

    std::stringstream ss;
    ss << i.rdbuf();
    nlohmann::json j = nlohmann::json::parse(ss);
    return codec::decode<TestFile>(j);
}

} // namespace mentor::kernel
