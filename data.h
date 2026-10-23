#pragma once

#include <filesystem>

#include "scan.h"

namespace HashGuard
{
    bool SaveBaseline(const ScanResult& result, const std::filesystem::path& outputPath);
}
