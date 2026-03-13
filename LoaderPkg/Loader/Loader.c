#include <Uefi.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>

#include <Guid/FileInfo.h>

#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

typedef void (*KERNEL_MAIN)(void);

EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage = NULL;

  Status = gBS->HandleProtocol(
    ImageHandle,
    &gEfiLoadedImageProtocolGuid,
    (VOID **)&LoadedImage);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;

  Status = gBS->HandleProtocol(
    LoadedImage->DeviceHandle,
    &gEfiSimpleFileSystemProtocolGuid,
    (VOID **)&FileSystem);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_PROTOCOL *RootFile = NULL;

  Status = FileSystem->OpenVolume(FileSystem, &RootFile);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_PROTOCOL *KernelFile = NULL;

  Status = RootFile->Open(
    RootFile,
    &KernelFile,
    L"kernel.bin",
    EFI_FILE_MODE_READ,
    0);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_INFO *FileInfo = NULL;
  UINTN InfoSize = SIZE_OF_EFI_FILE_INFO + 200;

  FileInfo = AllocatePool(InfoSize);

  Status = KernelFile->GetInfo(
    KernelFile,
    &gEfiFileInfoGuid,
    &InfoSize,
    FileInfo);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  UINTN KernelSize = FileInfo->FileSize;

  Print(L"Kernel size: %u bytes\n", KernelSize);

  VOID *KernelBuffer = AllocatePool(KernelSize);

  Status = KernelFile->Read(
    KernelFile,
    &KernelSize,
    KernelBuffer);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Print(L"Kernel loaded at 0x%p\n", KernelBuffer);

  KERNEL_MAIN KernelMain = (KERNEL_MAIN)KernelBuffer;

  Print(L"Jumping to kernel...\n");

  KernelMain();

  Print(L"Done\n");

  while (1)
    ;

  return Status;
}