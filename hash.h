#pragma once

#include <filesystem>
#include <string>

namespace HashGuard
{
	std::string CalculateSHA256(const std::filesystem::path& filePath);
}