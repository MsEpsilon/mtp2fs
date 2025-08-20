#include <Windows.h>
#include <PortableDeviceApi.h>
#include <print>
#include <iostream>
#include <wrl/client.h>
#include <unordered_map>
#include <codecvt>

#include "Com.hpp"
#include "Funcs.hpp"

#pragma comment(lib, "PortableDeviceGUIDs.lib")

using namespace Microsoft::WRL;

enum class ManageResult
{
	Error,
	NoDevices,
	OK
};

ManageResult manage(bool show)
{
	ComPtr<IPortableDeviceManager> manager;

	HRESULT r = CoCreateInstance(
		CLSID_PortableDeviceManager,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(manager.GetAddressOf())
	);

	if(FAILED(r) || manager == nullptr)
	{
		std::println(std::cerr, "PortableDeviceManager creation failed.");
		return ManageResult::Error;
	}

	DWORD numDeviceIDs = 0;
	DWORD numPrivDeviceIDs = 0;

	manager->GetDevices(nullptr, &numDeviceIDs);
	manager->GetPrivateDevices(nullptr, &numPrivDeviceIDs);

	if(numDeviceIDs == 0 && numPrivDeviceIDs == 0)
	{
		return ManageResult::NoDevices;
	}

	std::unique_ptr<wchar_t*[]> ids = std::make_unique<wchar_t*[]>(numDeviceIDs);
	std::unique_ptr<wchar_t*[]> privIDs = std::make_unique<wchar_t*[]>(numPrivDeviceIDs);

	manager->GetDevices(ids.get(), &numDeviceIDs);
	manager->GetPrivateDevices(privIDs.get(), &numPrivDeviceIDs);

	auto showDevice = [&manager](wchar_t* id)
		{
			DWORD deviceNameLen = 0;
			manager->GetDeviceFriendlyName(id, nullptr, &deviceNameLen);

			std::unique_ptr<wchar_t[]> name = std::make_unique<wchar_t[]>(deviceNameLen);

			HRESULT r = manager->GetDeviceFriendlyName(id, name.get(), &deviceNameLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to open device {}", ws2s(id));
			}

			DWORD deviceDescLen = 0;
			manager->GetDeviceDescription(id, nullptr, &deviceDescLen);

			std::unique_ptr<wchar_t[]> desc = std::make_unique<wchar_t[]>(deviceDescLen);

			r = manager->GetDeviceDescription(id, desc.get(), &deviceDescLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to read description for {}", ws2s(desc.get()));
			}

			std::println("\t{}:{} ", ws2s(name.get()), ws2s(desc.get()));
		}
	;

	if(show)
	{
		if(numDeviceIDs > 0)
		{
			std::println("Connected MTP devices:");
			for(DWORD i = 0; i < numDeviceIDs; ++i)
			{
				showDevice(ids[i]);
			}
		}

		if(numPrivDeviceIDs > 0)
		{
			std::println("Connected private MTP devices:");
			for(DWORD i = 0; i < numPrivDeviceIDs; ++i)
			{
				showDevice(privIDs[i]);
			}
		}
	}
	return ManageResult::OK;
}

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

	auto r1 = manage(showList);

	switch(r1)
	{
	case ManageResult::Error:
		return -1;
	case ManageResult::NoDevices:
		std::println("No MTP devices detected.");
		return -2;
	case ManageResult::OK:
		break;
	default:
		std::unreachable();
	}

	return 0;
}