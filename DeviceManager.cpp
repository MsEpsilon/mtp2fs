#include "DeviceManager.hpp"
#include <iostream>
#include <print>

#include "Funcs.hpp"

using namespace mtp2fs;
using namespace Microsoft::WRL;

DeviceManager::DeviceManager()
{
	_r = CoCreateInstance(
		CLSID_PortableDeviceManager,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&_manager)
	);
}

std::expected<std::vector<Device>, DeviceManager::DeviceEnumerationFailure> DeviceManager::EnumerateDevices()
{
	if(FAILED(_r) || _manager == nullptr)
	{
		return std::unexpected(DeviceEnumerationFailure::PortableDeviceManager_Creation_Fail);
	}

	std::vector<Device> result;

	DWORD numDeviceIDs = 0;
	DWORD numPrivDeviceIDs = 0;

	_manager->GetDevices(nullptr, &numDeviceIDs);
	_manager->GetPrivateDevices(nullptr, &numPrivDeviceIDs);

	if(numDeviceIDs == 0 && numPrivDeviceIDs == 0)
	{
		return std::unexpected(DeviceEnumerationFailure::OK_NoDevices);
	}

	std::unique_ptr<wchar_t*[]> ids = std::make_unique<wchar_t*[]>(numDeviceIDs);
	std::unique_ptr<wchar_t*[]> privIDs = std::make_unique<wchar_t*[]>(numPrivDeviceIDs);

	_manager->GetDevices(ids.get(), &numDeviceIDs);
	_manager->GetPrivateDevices(privIDs.get(), &numPrivDeviceIDs);

	auto showDevice = [this](wchar_t* id) -> Device
		{
			DWORD deviceNameLen = 0;
			this->_manager->GetDeviceFriendlyName(id, nullptr, &deviceNameLen);

			std::unique_ptr<wchar_t[]> name = std::make_unique<wchar_t[]>(deviceNameLen);

			HRESULT r = this->_manager->GetDeviceFriendlyName(id, name.get(), &deviceNameLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to open device {}", ws2s(id));
			}

			DWORD deviceDescLen = 0;
			this->_manager->GetDeviceDescription(id, nullptr, &deviceDescLen);

			std::unique_ptr<wchar_t[]> desc = std::make_unique<wchar_t[]>(deviceDescLen);

			r = this->_manager->GetDeviceDescription(id, desc.get(), &deviceDescLen);
			if(FAILED(r))
			{
				std::println(std::cerr, "Failed to read description for {}", ws2s(desc.get()));
			}

			return Device{
				std::wstring(id),
				std::wstring(name.get()),
				std::wstring(desc.get())
			};
		}
	;
	if(numDeviceIDs > 0)
	{
		for(DWORD i = 0; i < numDeviceIDs; ++i)
		{
			result.push_back(showDevice(ids[i]));
		}
	}

	result.push_back(
		Device
		{
			L"",
			L"",
			L""
		}
	);

	if(numPrivDeviceIDs > 0)
	{
		for(DWORD i = 0; i < numPrivDeviceIDs; ++i)
		{
			result.push_back(showDevice(privIDs[i]));
		}
	}

	return result;
}
