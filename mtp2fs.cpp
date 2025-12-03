#include <Windows.h>
#include <PortableDeviceApi.h>
#include <print>
#include <iostream>
#include <wrl/client.h>
#include <unordered_map>
#include <codecvt>
#include <algorithm>

#include "Com.hpp"
#include "Funcs.hpp"
#include "DeviceManager.hpp"
#include "MediaDevice.hpp"
#include "Filesystem.hpp"

#pragma comment(lib, "PortableDeviceGUIDs.lib")

using namespace Microsoft::WRL;
using namespace mtp2fs;
using namespace mtp2fs::fs;

enum class ManageResult
{
	Error,
	NoDevices,
	OK
};

auto main(int numArgs, char **ppArgs) -> int
{
	COM c_;

	std::unordered_map<std::string, int> argsDict =
	{
		{"--help",1},
		{"-h", 1},
		{"/h", 1},
		{"--mount", 2},
		{"-m", 2},
		{"/m", 2},
		{"--list", 3},
		{"-l", 3},
		{"/l", 3}
	};

	bool showList = false;

	std::string requiredDevice;
	std::string requiredDrive;

	for(int i = 1; i < numArgs; ++i)
	{
		switch(argsDict[ppArgs[i]])
		{
		case 1:
			std::println("mtp2fs - Mounts a MTP device to a file system.");
			std::println("Usage: mtp2fs [arguments]");
			std::println("Arguments:");
			std::println("--help, -h, /h : Shows this help message.");
			std::println("--mount <name> <drive letter> (-m, /h): Mounts a MTP device to a specified drive.");
			std::println("--list, -l, /l : Lists MTP devices.");
			return 0;
		case 2:
			if (!(numArgs > i + 1))
			{
				std::println("--mount requires a device name.");
				return -3;
			}
			if (!(numArgs > i + 2))
			{
				std::println("--mount requires a device letter");
				return -4;
			}

			requiredDevice = ppArgs[i + 1];
			requiredDrive = ppArgs[i + 2];

			i += 2;
			break;
		case 3:
			showList = true;
			break;
		default:
			std::println("No arguments specified! Use --help .");
			return 0;
		}
	}

	DeviceManager manager;
	
	auto deviceList = manager.EnumerateDevices();
	 
	int ret = 0;

	//Enumerate devices:
	auto devices = deviceList.and_then([showList](std::vector<Device> devices) -> decltype(deviceList)
		{
			if(showList)
			{
				std::println("Connected MTP devices:");
				for(auto& device : devices)
				{
					if(device.ID == L"")
					{
						std::println("Private MTP devices:");
					}
					else
					{
						std::println("\t {}:{}", ws2s(device.Name),  ws2s(device.Desc));
					}
				}
			}

			return std::expected<std::vector<Device>, DeviceManager::DeviceEnumerationFailure>(devices);
		}
	).or_else([&ret](DeviceManager::DeviceEnumerationFailure fail) -> decltype(deviceList)
		{
			switch(fail)
			{
			case DeviceManager::DeviceEnumerationFailure::PortableDeviceManager_Creation_Fail:
				std::println("IPortableDeviceManager creation failed!");
				ret = -1;
				break;
			case DeviceManager::DeviceEnumerationFailure::OK_NoDevices:
				std::println("No MTP devices detected.");
				ret = -2;
				break;
			default:
				std::println("Unknown error.");
				ret = -500;
				break;
			}

			return std::unexpected<decltype(fail)>(fail);
		}
	);

	if (showList) return 0;
	if (!devices.has_value()) return ret;

	//Select device:
	struct DeviceEntry
	{
		std::string Name;
		std::wstring ID;
		int Index;
	};

	std::vector<DeviceEntry> dst;
	
	for (auto& device : devices.value())
	{
		auto name = ws2s(device.Name);
		dst.emplace_back( name, device.ID, EditDistance(requiredDevice, name));
	}

	std::sort(dst.begin(), dst.end(), [](const DeviceEntry& a, const DeviceEntry& b) -> bool
		{
			return a.Index < b.Index;
		}
	);

	std::println("Selecting device {}", dst[0].Name);

	//Open device.
	MediaDevice device(dst[0].ID);
	if (!device.IsValid()) return -3 + device.Error();

	//Create filesystem
	if(requiredDrive.length() == 1)
		std::println("Creating filesystem at drive {}:\\\\", requiredDrive);
	else
		std::println("Creating filesystem at path {}", requiredDrive);

	Filesystem fs(requiredDrive);
	if (fs.IsValid())
		Sleep(10000);
	else
		return -5;

	return ret;
}