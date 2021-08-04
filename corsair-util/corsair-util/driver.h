#pragma once
#pragma warning (disable : 4047 4100 4024 4022)
#include <ntifs.h>

#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__);
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);
NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject);


// events
PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO imageInfo);

// data
ULONG ClientDLLAddress;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

// communication
#define IO_GET_CLIENT_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4242, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_READ_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4243, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_WRITE_REQUEST CTL_CODE(FILE_DEVICE_UNKNOWN, 0x4244, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
NTSTATUS CreateCall(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS CloseCall(PDEVICE_OBJECT deviceObject, PIRP irp);
NTSTATUS IoControl(PDEVICE_OBJECT deviceObject, PIRP irp);

typedef struct _KERNEL_READ_REQUEST {
	ULONG processId;
	ULONG address;
	PVOID pBuff;
	ULONG size;
} KERNEL_READ_REQUEST, * PKERNEL_READ_REQUEST;

typedef struct _KERNEL_WRITE_REQUEST {
	ULONG processId;
	ULONG address;
	PVOID pBuff;
	ULONG size;
} KERNEL_WRITE_REQUEST, * PKERNEL_WRITE_REQUEST;

// memory
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);
NTSTATUS KernelReadVirtualMemory(PEPROCESS process, PVOID sourceAddr, PVOID targetAddr, SIZE_T size);
NTSTATUS KernelWriteVirtualMemory(PEPROCESS process, PVOID sourceAddr, PVOID targetAddr, SIZE_T size);
