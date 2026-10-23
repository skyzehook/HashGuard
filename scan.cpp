#include "scan.h"
#include "hash.h"

#include <algorithm>
#include <stdexcept>
#include <system_error>

namespace HashGuard
{
    ScanResult ScanDirectory(
        const std::filesystem::path& directory)
    {
        namespace fs = std::filesystem;

        ScanResult result;

        std::error_code ec;

        if (!fs::is_directory(directory, ec))
        {
            throw std::runtime_error("invalid or inaccessible directory");
        }

        fs::recursive_directory_iterator it(directory, fs::directory_options::skip_permission_denied, ec);

        fs::recursive_directory_iterator end;

        if (ec)
        {
            throw fs::filesystem_error("cannot start directory scan", directory, ec);
        }

        while (it != end)
        {
            const fs::path currentPath = it->path();

            ec.clear();
            const bool regularFile = it->is_regular_file(ec);

            if (ec)
            {
                result.errors.push_back({currentPath,ec.message()});
            }


            else if (regularFile)
            {
                try
                {
                    FileInfo info;

                    info.path = fs::relative(currentPath, directory);

                    info.size = fs::file_size(currentPath);

                    info.sha256 = CalculateSHA256(currentPath);

                    result.files.push_back(std::move(info));

                }
                catch (const std::exception& error)
                {
                    result.errors.push_back({currentPath,error.what()});
                }
            }

            ec.clear();
            it.increment(ec);

            if (ec)
            {
                result.errors.push_back({currentPath,"directory traversal: " + ec.message()});

                ec.clear();
            }
        }

        std::sort(result.files.begin(), result.files.end(), [](const FileInfo& a, const FileInfo& b)
            {
                return a.path.native() < b.path.native();
            }
        );

        return result;
    }
}
