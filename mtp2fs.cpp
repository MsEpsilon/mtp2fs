#include <Windows.h>
#include <PortableDeviceApi.h>
#include <print>
#include <iostream>
#include <wrl/client.h>
#include <unordered_map>
#include <codecvt>

#include "Com.hpp"
#include "Funcs.hpp"
#include "DeviceManager.hpp"

#pragma comment(lib, "PortableDeviceGUIDs.lib")

using namespace Microsoft::WRL;
using namespace Devices;

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
			break;
		case 2:
			break;
		case 3:
			showList = true;
			break;
		default:
			break;
		}
	}

	DeviceManager manager;
	
	auto deviceList = manager.EnumerateDevices();
	 
	int ret = 0;

	deviceList.and_then([showList](std::vector<Device> devices) -> decltype(deviceList)
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

			return std::expected<std::vector<Device>, DeviceManager::DeviceEnumerationFailure>();
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

	return ret;
}