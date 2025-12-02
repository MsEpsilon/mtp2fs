#include "MediaDevice.hpp"
#include "PortableDevice.h"
#include "Constants.hpp"
#include <print>

using namespace mtp2fs;

MediaDevice::MediaDevice(const std::wstring& deviceID) :
	_err(0)
{
	//https://learn.microsoft.com/en-us/windows/win32/api/portabledeviceapi/nf-portabledeviceapi-iportabledevice-open

	//COM Create IPortableDeviceValues
	HRESULT r = CoCreateInstance(CLSID_PortableDeviceValues,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_IPortableDeviceValues,
		&_client
	);

	if (FAILED(r))
	{
		std::println("CoCreateInstance(CLSID_PortableDeviceValues, ...) failed!");
		_err = -1;
	}

	//Set app client info
	if (SUCCEEDED(r))
	{
		//HRESULT err code is ignored here
		_client->SetStringValue(WPD_CLIENT_NAME, AppClientName.data());
		_client->SetUnsignedIntegerValue(WPD_CLIENT_MAJOR_VERSION, AppMajorVer);
		_client->SetUnsignedIntegerValue(WPD_CLIENT_MINOR_VERSION, AppMinorVer);
		_client->SetUnsignedIntegerValue(WPD_CLIENT_REVISION, AppRevisionVer);
	}

	r = _client->SetUnsignedIntegerValue(WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE, SECURITY_IMPERSONATION);
	if (FAILED(r))
	{
		std::println("Setting WPD_CLIENT_SECURITY_QUALITY_OF_SERVICE failed!");
		_err = -2;
	} 
	
	//COM create IPortableDevice
	r = CoCreateInstance(CLSID_PortableDeviceFTM,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&_device)
	);

	if (FAILED(r))
	{
		std::println("CoCreateInstance(CLSID_PortableDeviceFTM, ...) failed!");
		_err = -3;
	}

	//Open device
	r = _device->Open(deviceID.c_str(), _client.Get());
	if (r == E_ACCESSDENIED)
	{
		r = _client->SetUnsignedIntegerValue(WPD_CLIENT_DESIRED_ACCESS, GENERIC_READ);
		if (FAILED(r))
		{
			std::println("Setting WPD_CLIENT_DESIRED_ACCESS failed!");
			_err = -3;
		}

		r = _device->Open(deviceID.c_str(), _client.Get());
		if (FAILED(r))
		{
			std::println("Failed to open the selected device in read-only mode!");
			_err = -4;
		}
		else std::println("Opening as read-only.");
	}

	_err = 0;
}

bool MediaDevice::IsValid() const noexcept
{
	return _err >= 0;
}

int MediaDevice::Error() const noexcept
{
	return _err;
}