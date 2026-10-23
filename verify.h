#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace HashGuard
{
    struct VerifyResult
    {
        std::vector<std::string> unchanged;
        std::vector<std::string> modified;
        std::vector<std::string> added;
        std::vector<std::string> deleted;
    };

    VerifyResult VerifyDirectory(const std::filesystem::path& directory, const std::filesystem::path& dataPath);
}
