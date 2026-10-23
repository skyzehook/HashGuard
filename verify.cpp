#include "verify.h"
#include "scan.h"
#include "json.hpp"

#include <fstream>
#include <map>
#include <stdexcept>

namespace HashGuard
{
    VerifyResult VerifyDirectory(
        const std::filesystem::path& directory,
        const std::filesystem::path& dataPath)
    {
        std::ifstream input(dataPath, std::ios::binary);

        if (!input)
        {
            throw std::runtime_error("data.json not found");
        }

        nlohmann::json data;
        input >> data;

        if (!data.is_object() || !data.contains("version") || data.at("version") != 1 || !data.contains("files") || !data.at("files").is_array())
        {
            throw std::runtime_error("invalid baseline format");
        }


        std::map<std::string, std::string> oldFiles;

        for (const auto& file : data.at("files"))
        {
            std::string path = file.at("path").get<std::string>();
            std::string hash = file.at("sha256").get<std::string>();

            if (path.empty() || hash.size() != 64)
            {
                throw std::runtime_error("invalid baseline entry");
            }

            if (!oldFiles.emplace(path, hash).second)
            {
                throw std::runtime_error("duplicate baseline path");
            }
        }


        ScanResult scan = ScanDirectory(directory);

        if (!scan.errors.empty())
        {
            throw std::runtime_error(
                "scan failed, verification incomplete");
        }


        VerifyResult result;

        std::map<std::string, std::string> newFiles;

        for (const auto& file : scan.files)
        {
            newFiles[file.path.generic_string()] = file.sha256;
        }


        for (const auto& [path, hash] : newFiles)
        {
            auto it = oldFiles.find(path);

            if (it == oldFiles.end())
            {
                result.added.push_back(path);
            }
            else if (it->second != hash)
            {
                result.modified.push_back(path);
            }
            else
            {
                result.unchanged.push_back(path);
            }
        }


        for (const auto& [path, hash] : oldFiles)
        {
            if (newFiles.find(path) == newFiles.end())
            {
                result.deleted.push_back(path);
            }
        }


        return result;
    }
}
