#pragma once
#include <Windows.h>

#define IO_GET_CLIENT_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4242, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4243, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4244, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _KERNEL_REQUEST {
	// address to read - no need to get module base for offset
	ULONG address;
	// pointer storing data
	void * data;
	// size of data in pointer
	ULONG size;
} KERNEL_REQUEST;

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

	void ReadMemory(KERNEL_REQUEST * kernelRequest) {
		if (hDriver == INVALID_HANDLE_VALUE) {
			return;
		}
		DWORD bytes;

		DeviceIoControl(
			hDriver,
			IO_READ_REQUEST,
			kernelRequest,
			sizeof(KERNEL_REQUEST),
			kernelRequest,
			sizeof(KERNEL_REQUEST),
			&bytes,
			NULL
		);
	}

	void WriteMemory(KERNEL_REQUEST * kernelRequest) {
		if (hDriver == INVALID_HANDLE_VALUE) {
			return;
		}
		DWORD bytes;

		DeviceIoControl(
			hDriver,
			IO_WRITE_REQUEST,
			kernelRequest,
			sizeof(KERNEL_REQUEST),
			kernelRequest,
			sizeof(KERNEL_REQUEST),
			&bytes,
			NULL
		);
	}
};

