#include <Windows.h>
#include <PortableDeviceApi.h>
#include <print>
#include <iostream>
#include <wrl/client.h>
#include <unordered_map>

#pragma comment(lib, "PortableDeviceGUIDs.lib")

using namespace Microsoft::WRL;

class COM
{
public:
	COM()
	{
		HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

		if(FAILED(r))
			std::println(std::cerr, "CoInitialize failed.");
	}

	~COM()
	{
		CoUninitialize();
	}
};

enum class ManageResult
{
	Error,
	NoDevices,
	OK
};

ManageResult manage()
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

	DWORD deviceIDs = 0;

	manager->GetDevices(nullptr, &deviceIDs);

	if(deviceIDs == 0)
	{
		return ManageResult::NoDevices;
	}
}

auto main(int numArgs, char **ppArgs) -> int
{
	COM c_;

	auto r1 = manage();

	std::unordered_map<std::string, int> argsDict =
	{
		{"--help",1},
		{"-h", 1},
		{"/h", 1},
		{"--mount", 1},
		{"-m", 1},
		{"/m", 1}
	};

	std::println("{}", ppArgs[0]);

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