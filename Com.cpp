#include "Com.hpp"
#include <combaseapi.h>
#include <iostream>

COM::COM()
{
	HRESULT r = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	if(FAILED(r))
		std::println(std::cerr, "CoInitialize failed.");
}

COM::~COM()
{
	CoUninitialize();
}