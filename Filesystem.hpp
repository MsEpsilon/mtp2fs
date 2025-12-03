#pragma once
#include "dokan/dokan/dokan.h"
#include <filesystem>

namespace mtp2fs::fs
{
	extern DOKAN_OPERATIONS memfs_operations;

	class Filesystem
	{
		DOKAN_HANDLE _dkHandle;
		std::wstring _mountPoint;
		bool _valid;
	public:
		Filesystem(const std::filesystem::path& p);
		void Wait();
		bool IsValid() const noexcept;
		~Filesystem();
	};
}