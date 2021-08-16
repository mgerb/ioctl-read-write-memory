#pragma warning (disable : 4047 4100 4024 4022)
#include "driver.h"

/*
	create driver:
	sc create <driver name> type= kernel binpath="path to driver"

	delete driver:
	sc delete <driver name>

	start driver:
	sc start <driver name>

	stop driver:
	sc stop <driver name>

	enable test signing (requires reboot):
	bcdedit /set testsigning on
*/

const wchar_t* client_path = L"ac_client.exe";

NTSTATUS KernelReadVirtualMemory(PEPROCESS process, PVOID sourceAddr, PVOID targetAddr, SIZE_T size) {
	PSIZE_T bytes;
	return MmCopyVirtualMemory(process, sourceAddr, PsGetCurrentProcess(), targetAddr, size, KernelMode, &bytes);
}

NTSTATUS KernelWriteVirtualMemory(PEPROCESS process, PVOID sourceAddr, PVOID targetAddr, SIZE_T size) {
	PSIZE_T bytes;
	return MmCopyVirtualMemory(PsGetCurrentProcess(), sourceAddr, process, targetAddr, size, KernelMode, &bytes);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistrtyPath)
{
	UNREFERENCED_PARAMETER(pRegistrtyPath);
	pDriverObject->DriverUnload = UnloadDriver;
	DebugMessage("Corsair Utility Driver Entry");

	PsSetLoadImageNotifyRoutine(ImageLoadCallback);

	RtlInitUnicodeString(&dev, L"\\Device\\corsair-util");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\corsair-util");

	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);
	DebugMessage("Corsair Utility Driver Unloaded");

	PsRemoveLoadImageNotifyRoutine(ImageLoadCallback);

	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

	return STATUS_SUCCESS;
}

PLOAD_IMAGE_NOTIFY_ROUTINE ImageLoadCallback(PUNICODE_STRING fullImageName, HANDLE processId, PIMAGE_INFO imageInfo)
{
	if (wcsstr(fullImageName->Buffer, client_path))
	{
		DebugMessage("executable found: %p \n", client_path);
		ClientDLLAddress = imageInfo->ImageBase;
		ClientProcessId = processId;
		DebugMessage("process id: %d \n", processId);
	}

	return STATUS_SUCCESS;
}

NTSTATUS CreateCall(PDEVICE_OBJECT deviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	DebugMessage("CreateCall\n");

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT deviceObject, PIRP irp)
{
	irp->IoStatus.Status = STATUS_SUCCESS;
	irp->IoStatus.Information = 0;

	IoCompleteRequest(irp, IO_NO_INCREMENT);

	DebugMessage("CloseCall\n");

	return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT deviceObject, PIRP irp)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG byteIO = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);

	ULONG controlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (controlCode == IO_GET_CLIENT_ADDRESS) {
		PULONG output = (PULONG)irp->AssociatedIrp.SystemBuffer;
		*output = ClientDLLAddress;
		status = STATUS_SUCCESS;
		byteIO = sizeof(*output);
	} else if (controlCode == IO_READ_REQUEST) {
		KERNEL_REQUEST * readInput = (KERNEL_REQUEST*) irp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(ClientProcessId, &process))) {
			DebugMessage("Reading memory address: %lu", readInput->address);
			DebugMessage("Reading memory pBuff: %lu", readInput->pBuff);
			DebugMessage("Reading memory size: %lu", readInput->size);
			KernelReadVirtualMemory(process, readInput->address, readInput->pBuff, readInput->size);
			status = STATUS_SUCCESS;
			byteIO = sizeof(KERNEL_REQUEST);
		}
	} else if (controlCode == IO_WRITE_REQUEST) {
		KERNEL_REQUEST * writeInput = (KERNEL_REQUEST*) irp->AssociatedIrp.SystemBuffer;
		PEPROCESS process;
		if (NT_SUCCESS(PsLookupProcessByProcessId(ClientProcessId, &process))) {
			DebugMessage("Writing memory address: %lu", writeInput->address);
			DebugMessage("Writing memory pBuff: %lu", writeInput->pBuff);
			DebugMessage("Writing memory size: %lu", writeInput->size);
			KernelWriteVirtualMemory(process, writeInput->pBuff, writeInput->address, writeInput->size);
			status = STATUS_SUCCESS;
			byteIO = sizeof(KERNEL_REQUEST);
		}
	} else {
		byteIO = 0;
	}

	irp->IoStatus.Status = status;
	irp->IoStatus.Information = byteIO;
	IoCompleteRequest(irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

