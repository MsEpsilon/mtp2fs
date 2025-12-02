#pragma once
#include "Device.hpp"

#include <PortableDeviceApi.h>
#include <wrl/client.h>

namespace mtp2fs
{
	class DeviceManager
	{
		Microsoft::WRL::ComPtr<IPortableDeviceManager> _manager;
		HRESULT _r;
	public:
		DeviceManager();
	
		enum class DeviceEnumerationFailure
		{
			Unspecified,
			OK_NoDevices,
			PortableDeviceManager_Creation_Fail
		};
	
		std::expected<std::vector<Device>, DeviceEnumerationFailure> EnumerateDevices();
	};
}