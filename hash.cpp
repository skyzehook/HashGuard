#include "hash.h"


#include <Windows.h>
#include <bcrypt.h>

#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>


#pragma comment (lib, "bcrypt.lib")

namespace HashGuard
{
	std::string CalculateSHA256(const std::filesystem::path& filePath)
	{
		BCRYPT_ALG_HANDLE algorithm = nullptr;
		BCRYPT_HASH_HANDLE hash = nullptr;


		auto cleanup = [&]()
		{
				if (hash)
					BCryptDestroyHash(hash);

				if (algorithm)
					BCryptCloseAlgorithmProvider(algorithm, 0);
		};


		try
		{
			std::ifstream file(filePath, std::ios::binary);

			if (!file)
				throw std::runtime_error("cannot open file");

			NTSTATUS status = BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0);

			if (!BCRYPT_SUCCESS(status))
				throw std::runtime_error("cannot initialize SHA-256");


			DWORD hashLength = 0;
			DWORD resultLength = 0;

			status = BCryptGetProperty(algorithm, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&hashLength), sizeof(hashLength), &resultLength, 0);

			if (!BCRYPT_SUCCESS(status))
				throw std::runtime_error("cannot get hash length");

			status = BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0);

			if (!BCRYPT_SUCCESS(status))
				throw std::runtime_error("cannot create hash");

			std::array<char, 65536> buffer{};


			while (file)
			{
				file.read(buffer.data(), buffer.size());

				std::streamsize bytesRead = file.gcount();

				if (bytesRead > 0)
				{
					status = BCryptHashData(hash, reinterpret_cast<PUCHAR>(buffer.data()), static_cast<ULONG>(bytesRead), 0);

					if (!BCRYPT_SUCCESS(status))
						throw std::runtime_error("hashing failed");
				}
			}


			if (file.bad())
				throw std::runtime_error("file read failed");

			std::vector<UCHAR> hashBytes(hashLength);

			status = BCryptFinishHash(hash, hashBytes.data(), hashLength, 0);

			if (!BCRYPT_SUCCESS(status))
				throw std::runtime_error("cannot finish hash");

			std::ostringstream output;

			output << std::hex<< std::setfill('0');

			for (UCHAR byte : hashBytes)
			{
				output << std::setw(2) << static_cast<int>(byte);
			}

			cleanup();
			return output.str();



		}
		catch (...)
		{
			cleanup();
			throw;
		}



	}

}