extern "C"{
#include <ntddk.h>
}

void Unload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING Win32Device;

	DbgPrint("Unload Driver!\n");

	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\TestDriver");

	IoDeleteSymbolicLink(&Win32Device);

	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS Create(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp,IO_NO_INCREMENT);

	DbgPrint("Create Device\n");

	return STATUS_SUCCESS;
}

NTSTATUS Close(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp,IO_NO_INCREMENT);

	DbgPrint("Close Device\n");

	return STATUS_SUCCESS;
}

NTSTATUS DefaultHandler(IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp,IO_NO_INCREMENT);

	DbgPrint("Default Handler\n");

	return Irp->IoStatus.Status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING DeviceName,Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	
	DbgPrint("Load Driver!\n");

	RtlInitUnicodeString(&DeviceName,L"\\Device\\TestDriver");
	RtlInitUnicodeString(&Win32Device,L"\\DosDevices\\TestDriver");

	for(unsigned i = 0;i <= IRP_MJ_MAXIMUM_FUNCTION;++i){
		DriverObject->MajorFunction[i] = DefaultHandler;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = Create;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = Close;

	DriverObject->DriverUnload = Unload;

	status = IoCreateDevice(DriverObject,10,&DeviceName,FILE_DEVICE_UNKNOWN,0,FALSE,&DeviceObject);

	if(!NT_SUCCESS(status)){
		return status;
	}
	if(!DeviceObject){
		return STATUS_UNEXPECTED_IO_ERROR;
	}

	DeviceObject->Flags |= DO_DIRECT_IO;

	status = IoCreateSymbolicLink(&Win32Device,&DeviceName);

	DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}