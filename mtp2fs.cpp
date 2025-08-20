#include <Windows.h>
#include <PortableDeviceApi.h>
#include <print>
#include <iostream>
#include <wrl/client.h>
#include <unordered_map>
#include <codecvt>

#include "Com.hpp"

#pragma comment(lib, "PortableDeviceGUIDs.lib")

using namespace Microsoft::WRL;

std::string ws2s(const std::wstring& wstr)
{
	int wLen = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), nullptr, 0, nullptr, nullptr);

	std::string result(wLen, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), wstr.length(), &result[0], wLen, nullptr, nullptr);

	return result;
}

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

	manager->GetDevices(nullptr, &numDeviceIDs);

	if(numDeviceIDs == 0)
	{
		return ManageResult::NoDevices;
	}

	std::unique_ptr<wchar_t*[]> ids = std::make_unique<wchar_t*[]>(numDeviceIDs);

	manager->GetDevices(ids.get(), &numDeviceIDs);

	if(show)
	{
		std::println("Connected MTP devices:");
		
		for(DWORD i = 0; i < numDeviceIDs; ++i)
		{
			DWORD deviceNameLen = 0;
			manager->GetDeviceFriendlyName(ids[i], nullptr, &deviceNameLen);

			std::unique_ptr<wchar_t[]> name = std::make_unique<wchar_t[]>(deviceNameLen);

			r = manager->GetDeviceFriendlyName(ids[i], name.get(), &deviceNameLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to open device {}", ws2s(ids[i]));
			}

			DWORD deviceDescLen = 0;
			manager->GetDeviceDescription(ids[i], nullptr, &deviceDescLen);

			std::unique_ptr<wchar_t[]> desc = std::make_unique<wchar_t[]>(deviceDescLen);
			
			r = manager->GetDeviceDescription(ids[i], desc.get(), &deviceDescLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to read description for {}", ws2s(desc.get()));
			}

			std::println("\t{}:{} ", ws2s(name.get()), ws2s(desc.get()));
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