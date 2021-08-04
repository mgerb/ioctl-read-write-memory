#pragma once
#include <Windows.h>

#define IO_GET_CLIENT_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4242, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

class KernelInterface {
public:
	HANDLE hDriver;

	KernelInterface(LPCSTR registryPath) {
		hDriver = CreateFileA(registryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}

	DWORD GetClientAddress() {
		if (hDriver == INVALID_HANDLE_VALUE) {
			return 0;
		}

		ULONG address;
		DWORD bytes;

		if (DeviceIoControl(hDriver, IO_GET_CLIENT_ADDRESS, &address, sizeof(address), &address, sizeof(address), &bytes, NULL)) {
			return address;
		}

		return 0;
	}
};

