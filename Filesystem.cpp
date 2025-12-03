#include "Filesystem.hpp"

using namespace mtp2fs;
using namespace mtp2fs::fs;

DOKAN_OPERATIONS mtp2fs::fs::memfs_operations{};
DOKAN_OPTIONS dokan_options{};

Filesystem::Filesystem(const std::filesystem::path& path) :
    _mountPoint(path.wstring()),
    _valid(false)
{
	//DOKAN_OPTIONS uses 16432 bytes on the stack??

	dokan_options.Version = DOKAN_VERSION;
	dokan_options.Options = DOKAN_OPTION_ALT_STREAM | DOKAN_OPTION_CASE_SENSITIVE;
	dokan_options.MountPoint = _mountPoint.c_str();
	dokan_options.SingleThread = false;
	dokan_options.Options |= DOKAN_OPTION_REMOVABLE;

	dokan_options.Timeout = 0;
	dokan_options.GlobalContext = reinterpret_cast<ULONG64>(this);
	
    NTSTATUS status =
        DokanCreateFileSystem(&dokan_options, &memfs_operations, &_dkHandle);
    switch (status) {
    case DOKAN_SUCCESS:
        break;
    case DOKAN_ERROR:
        std::printf("Dokan error.");
        break;
    case DOKAN_DRIVE_LETTER_ERROR:
        std::printf("Bad Drive letter");
        break;
    case DOKAN_DRIVER_INSTALL_ERROR:
        std::printf("Can't install driver");
        break;
    case DOKAN_START_ERROR:
        std::printf("Driver something wrong");
        break;
    case DOKAN_MOUNT_ERROR:
        std::printf("Can't assign a drive letter");
        break;
    case DOKAN_MOUNT_POINT_ERROR:
        std::printf("Mount point error");
        break;
    case DOKAN_VERSION_ERROR:
        std::printf("Version error");
        break;
    default:
        std::printf("Unknown error");
        break;
    }

    _valid = true;
}

void Filesystem::Wait()
{
    DokanWaitForFileSystemClosed(_dkHandle, INFINITE);
    // Release instance resources
    DokanCloseHandle(_dkHandle);
}

bool Filesystem::IsValid() const noexcept
{
    return _valid;
}

Filesystem::~Filesystem()
{
    DokanRemoveMountPoint(_mountPoint.c_str());
}