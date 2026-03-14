#include <Uefi.h>

#include <Protocol/SimpleFileSystem.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/GraphicsOutput.h>

#include <Guid/FileInfo.h>

#include <Library/PcdLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

typedef struct FRAME_BUFFER_INFO {
  EFI_PHYSICAL_ADDRESS FrameBuffer;
  UINT64 FrameBufferSize;
  UINT32 Width;
  UINT32 Height;
  UINT32 PixelsPerScanLine;
  UINT32 BitsPerPixel;
} FRAME_BUFFER_INFO;

typedef VOID (*KERNEL_MAIN)(FRAME_BUFFER_INFO *);

STATIC EFI_LOADED_IMAGE_PROTOCOL *sLoadedImageProtocol = NULL;
STATIC EFI_GRAPHICS_OUTPUT_PROTOCOL *sGraphicsOutputProtocol = NULL;

STATIC FRAME_BUFFER_INFO sFrameBufferInfo = {0};

STATIC
EFI_STATUS
EFIAPI
RetrieveRequiredProtocols(IN EFI_HANDLE ImageHandle) {
  EFI_STATUS Status = EFI_SUCCESS;

  Status = gBS->HandleProtocol(
    ImageHandle,
    &gEfiLoadedImageProtocolGuid,
    (VOID **)&sLoadedImageProtocol);

  if (EFI_ERROR(Status)) {
    Print(L"Failed retrieving EFI_LOADED_IMAGE_PROTOCOL\n");
    return Status;
  }

  Status = gBS->LocateProtocol(
    &gEfiGraphicsOutputProtocolGuid,
    NULL,
    (VOID **)&sGraphicsOutputProtocol);

  if (EFI_ERROR(Status)) {
    Print(L"Failed retrieving EFI_GRAPHICS_OUTPUT_PROTOCOL\n");
    return Status;
  }

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
RetrieveFrameBufferInformation(VOID) {
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GraphicsOutputModeInfo = sGraphicsOutputProtocol->Mode->Info;

  sFrameBufferInfo.FrameBuffer = sGraphicsOutputProtocol->Mode->FrameBufferBase;
  sFrameBufferInfo.FrameBufferSize = sGraphicsOutputProtocol->Mode->FrameBufferSize;

  sFrameBufferInfo.Width = GraphicsOutputModeInfo->HorizontalResolution;
  sFrameBufferInfo.Height = GraphicsOutputModeInfo->VerticalResolution;
  sFrameBufferInfo.PixelsPerScanLine = GraphicsOutputModeInfo->PixelsPerScanLine;
  sFrameBufferInfo.BitsPerPixel = 32;

  return Status;
}

/*
STATIC
EFI_STATUS
EFIAPI
SetHighestAvailableGraphicsMode(VOID) {
  EFI_STATUS Status = EFI_SUCCESS;

  UINT64 ModeIndex = 0;
  UINT64 ModeCount = sGraphicsOutputProtocol->Mode->MaxMode;

  while (ModeIndex < ModeCount) {

    UINT64 ModeSize = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *ModeInfo = NULL;

    Status = sGraphicsOutputProtocol->QueryMode(sGraphicsOutputProtocol, ModeIndex, &ModeSize, &ModeInfo);

    if (EFI_ERROR(Status)) {
      return Status;
    }

    // TODO
    if (ModeInfo->HorizontalResolution == 1024 && ModeInfo->VerticalResolution == 768) {

      Status = sGraphicsOutputProtocol->SetMode(sGraphicsOutputProtocol, ModeIndex);

      if (EFI_ERROR(Status)) {
        return Status;
      }

      break;
    }

    ModeIndex++;
  }

  return Status;
}
*/

EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status = EFI_SUCCESS;

  Status = gBS->SetWatchdogTimer(0, 0, 0, NULL);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = RetrieveRequiredProtocols(ImageHandle);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Print(L"Image loaded at: 0x%p\n", sLoadedImageProtocol->ImageBase); // TODO: remove me..

  PXE_VOLATILE UINT64 *MarkerPtr = (UINT64 *)0x10000;
  PXE_VOLATILE UINT64 *ImageBase = (UINT64 *)0x10008;

  *ImageBase = (UINT64)sLoadedImageProtocol->ImageBase;
  *MarkerPtr = 0xDEADBEEF;

  Status = RetrieveFrameBufferInformation();

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Status = SetHighestAvailableGraphicsMode();

  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }

  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;

  Status = gBS->HandleProtocol(
    sLoadedImageProtocol->DeviceHandle,
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
    L"KERNEL.BIN",
    EFI_FILE_MODE_READ,
    0);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_INFO *FileInfo = NULL;
  UINT64 InfoSize = SIZE_OF_EFI_FILE_INFO + 200;

  FileInfo = AllocatePool(InfoSize);

  Status = KernelFile->GetInfo(
    KernelFile,
    &gEfiFileInfoGuid,
    &InfoSize,
    FileInfo);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  UINT64 KernelSize = FileInfo->FileSize;

  Print(L"Kernel size: %u bytes\n", KernelSize);

  UINT64 KernelPages = EFI_SIZE_TO_PAGES(KernelSize);
  EFI_PHYSICAL_ADDRESS KernelAddress = 0;

  Status = gBS->AllocatePages(
    AllocateAnyPages,
    EfiLoaderData,
    KernelPages,
    &KernelAddress);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = KernelFile->Read(
    KernelFile,
    &KernelSize,
    (VOID *)KernelAddress);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Print(L"Kernel loaded at 0x%p\n", KernelAddress);

  // UINT64 MapSize = 0;
  // EFI_MEMORY_DESCRIPTOR *Map = NULL;
  // UINT64 MapKey = 0;
  // UINT64 DescriptorSize = 0;
  // UINT32 DescriptorVersion = 0;

  // Status = gBS->GetMemoryMap(
  //   &MapSize,
  //   Map,
  //   &MapKey,
  //   &DescriptorSize,
  //   &DescriptorVersion);

  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }

  // MapSize += DescriptorSize * 8;
  // Map = AllocatePool(MapSize);

  // Status = gBS->GetMemoryMap(
  //   &MapSize,
  //   Map,
  //   &MapKey,
  //   &DescriptorSize,
  //   &DescriptorVersion);

  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }

  // Status = gBS->ExitBootServices(ImageHandle, MapKey);

  // if (EFI_ERROR(Status)) {
  //   return Status;
  // }

  KERNEL_MAIN KernelMain = (KERNEL_MAIN)KernelAddress;

  // Print(L"Jumping to kernel...\n");

  KernelMain(&sFrameBufferInfo);

  // Print(L"Done\n");

  return Status;
}