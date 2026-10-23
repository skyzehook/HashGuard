#define _SILENCE_CXX20_U8PATH_DEPRECATION_WARNING

#include <iostream>
#include <filesystem>
#include <string>
#include <chrono>

#include "hash.h"
#include "scan.h"
#include "data.h"
#include "verify.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "usage: hashguard <command> <path>\n";
        std::cout << "commands: hash, scan, init, verify, help\n";
        return 0;
    }

    std::string command = argv[1];

    if (command == "help" || command == "--help")
    {
        std::cout << "hashguard commands:\n\n";
        std::cout << "hash <file>        calculate sha256\n";
        std::cout << "scan <directory>   scan all files\n";
        std::cout << "init <directory>   create baseline\n";
        std::cout << "verify <directory> verify files\n";
        return 0;
    }

    if (command != "hash" &&
        command != "scan" &&
        command != "init" &&
        command != "verify")
    {
        std::cout << "invalid command\n";
        return 0;
    }

    if (argc != 3)
    {
        std::cout << "invalid arguments\n";
        std::cout << "usage: hashguard <command> <path>\n";
        return 0;
    }

    try
    {
        std::filesystem::path filePath =
            std::filesystem::u8path(argv[2]);

        auto start = std::chrono::steady_clock::now();

        if (command == "hash")
        {
            if (!std::filesystem::is_regular_file(filePath))
            {
                std::cout << "file not found\n";
                return 0;
            }

            std::string hash = HashGuard::CalculateSHA256(filePath);

            std::cout << "hash -> " << hash << "\n";
        }

        else if (command == "scan")
        {
            if (!std::filesystem::is_directory(filePath))
            {
                std::cout << "directory not found\n";
                return 0;
            }

            HashGuard::ScanResult result =
                HashGuard::ScanDirectory(filePath);

            for (const auto& file : result.files)
            {
                std::cout << "\nfile -> " << file.path.string() << "\n";
                std::cout << "size -> " << file.size << " bytes\n";
                std::cout << "hash -> " << file.sha256 << "\n";
            }

            for (const auto& error : result.errors)
            {
                std::cout << "\nerror -> " << error.path.string() << "\n";
                std::cout << "reason -> " << error.message << "\n";
            }

            std::cout << "\nscan completed\n";
            std::cout << "files -> " << result.files.size() << "\n";
            std::cout << "errors -> " << result.errors.size() << "\n";
        }

        else if (command == "init")
        {
            if (!std::filesystem::is_directory(filePath))
            {
                std::cout << "directory not found\n";
                return 0;
            }

            std::filesystem::path outputPath =
                std::filesystem::absolute(filePath).parent_path() / "data.json";

            if (std::filesystem::exists(outputPath))
            {
                std::cout << "data.json already exists\n";
                return 0;
            }

            HashGuard::ScanResult result =
                HashGuard::ScanDirectory(filePath);

            if (HashGuard::SaveBaseline(result, outputPath))
            {
                std::cout << "baseline created\n";
                std::cout << "files -> " << result.files.size() << "\n";
                std::cout << "saved -> " << outputPath.string() << "\n";
            }
            else
            {
                std::cout << "baseline failed\n";
                return 0;
            }
        }

        else if (command == "verify")
        {
            if (!std::filesystem::is_directory(filePath))
            {
                std::cout << "directory not found\n";
                return 0;
            }

            std::filesystem::path dataPath =
                std::filesystem::absolute(filePath).parent_path() / "data.json";

            HashGuard::VerifyResult result =
                HashGuard::VerifyDirectory(filePath, dataPath);

            for (const auto& file : result.unchanged)
            {
                std::cout << "unchanged -> " << file << "\n";
            }

            for (const auto& file : result.modified)
            {
                std::cout << "modified -> " << file << "\n";
            }

            for (const auto& file : result.added)
            {
                std::cout << "added -> " << file << "\n";
            }

            for (const auto& file : result.deleted)
            {
                std::cout << "deleted -> " << file << "\n";
            }

            std::cout << "\nverification completed\n";
            std::cout << "unchanged -> " << result.unchanged.size() << "\n";
            std::cout << "modified -> " << result.modified.size() << "\n";
            std::cout << "added -> " << result.added.size() << "\n";
            std::cout << "deleted -> " << result.deleted.size() << "\n";
        }

        auto end = std::chrono::steady_clock::now();

        double elapsed = std::chrono::duration<double, std::milli>(
            end - start
        ).count();

        std::cout << "\ntime -> " << elapsed << " ms\n";
    }

    catch (const std::exception& error)
    {
        std::cout << "error -> " << error.what() << "\n";
        return 0;
    }

    return 0;
}
