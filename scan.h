#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>

namespace HashGuard
{
    struct FileInfo
    {
        std::filesystem::path path;
        std::string sha256;
        std::uintmax_t size = 0;
    };

    struct ScanError
    {
        std::filesystem::path path;
        std::string message;
    };

    struct ScanResult
    {
        std::vector<FileInfo> files;
        std::vector<ScanError> errors;
    };

    ScanResult ScanDirectory(
        const std::filesystem::path& directory
    );
}
