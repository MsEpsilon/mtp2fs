#pragma once
#include <string>
#include <vector>
#include <expected>

namespace Devices
{
	struct Device
	{
		std::wstring ID;
		std::wstring Name;
		std::wstring Desc;
	};
}