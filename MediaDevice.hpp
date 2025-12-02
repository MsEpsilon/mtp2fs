#pragma once
#include "Device.hpp"

#include <PortableDeviceApi.h>
#include <wrl/client.h>

namespace mtp2fs
{
    class MediaDevice
    {
        Microsoft::WRL::ComPtr<IPortableDeviceValues> _client;
        Microsoft::WRL::ComPtr<IPortableDevice> _device;
        int _err;
    public:
        MediaDevice(const std::wstring& deviceID);

        bool IsValid() const noexcept;

        ~MediaDevice() = default;
    };
}