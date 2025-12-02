#pragma once
#include <string>
#include <vector>
#include <expected>

namespace mtp2fs
{
	struct Device
	{
		std::wstring ID;
		std::wstring Name;
		std::wstring Desc;
	};
}