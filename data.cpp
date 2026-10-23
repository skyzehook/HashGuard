
#include "data.h"

#include <fstream>
#include <iostream>
#include "json.hpp"

namespace HashGuard
{
    bool SaveBaseline(const ScanResult& result, const std::filesystem::path& outputPath)
    {
        if (!result.errors.empty())
        {
            std::cerr << "scan has errors\n";
            return false;
        }

        nlohmann::json data;

        data["version"] = 1;
        data["files"] = nlohmann::json::array();

        for (const auto& file : result.files)
        {
            data["files"].push_back({ {"path", file.path.generic_string()}, {"size", file.size}, {"sha256", file.sha256} });
        }

        std::ofstream output(outputPath, std::ios::binary);

        if (!output)
        {
            std::cerr << "cannot create data file\n";
            return false;
        }

        output << data.dump(4);

        output.flush();

        if (!output)
        {
            std::cerr << "cannot write data file\n";
            return false;
        }

        return true;
    }
}
