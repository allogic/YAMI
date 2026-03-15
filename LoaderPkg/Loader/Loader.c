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

typedef struct FRAME_BUFFER {
  EFI_PHYSICAL_ADDRESS Buffer;
  UINT64 BufferSize;
  UINT32 Width;
  UINT32 Height;
  UINT32 PixelsPerScanLine;
  UINT32 BitsPerPixel;
} FRAME_BUFFER;

typedef struct BOOT_INFO {
  EFI_PHYSICAL_ADDRESS KernelAddress;
  UINT64 KernelSize;
  FRAME_BUFFER FrameBuffer;
  EFI_MEMORY_DESCRIPTOR *MemoryMap;
  UINT64 MemoryMapSize;
  UINT64 DescriptorSize;
} BOOT_INFO;

typedef VOID (*KERNEL_START)(BOOT_INFO *);

STATIC EFI_LOADED_IMAGE_PROTOCOL *sLoadedImageProtocol = NULL;
STATIC EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *sSimpleFileSystemProtocol = NULL;
STATIC EFI_GRAPHICS_OUTPUT_PROTOCOL *sGraphicsOutputProtocol = NULL;

STATIC BOOT_INFO sBootInfo = {0};

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
    return Status;
  }

  Status = gBS->LocateProtocol(
    &gEfiGraphicsOutputProtocolGuid,
    NULL,
    (VOID **)&sGraphicsOutputProtocol);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->HandleProtocol(
    sLoadedImageProtocol->DeviceHandle,
    &gEfiSimpleFileSystemProtocolGuid,
    (VOID **)&sSimpleFileSystemProtocol);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
SetHighestAvailableGraphicsMode(VOID) {
  EFI_STATUS Status = EFI_SUCCESS;

  for (UINT64 ModeIndex = 0; ModeIndex < sGraphicsOutputProtocol->Mode->MaxMode; ModeIndex++) {

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

STATIC
EFI_STATUS
EFIAPI
RetrieveFrameBufferInformation(VOID) {
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *GraphicsOutputModeInfo = sGraphicsOutputProtocol->Mode->Info;

  sBootInfo.FrameBuffer.Buffer = sGraphicsOutputProtocol->Mode->FrameBufferBase;
  sBootInfo.FrameBuffer.BufferSize = sGraphicsOutputProtocol->Mode->FrameBufferSize;

  sBootInfo.FrameBuffer.Width = GraphicsOutputModeInfo->HorizontalResolution;
  sBootInfo.FrameBuffer.Height = GraphicsOutputModeInfo->VerticalResolution;
  sBootInfo.FrameBuffer.PixelsPerScanLine = GraphicsOutputModeInfo->PixelsPerScanLine;
  sBootInfo.FrameBuffer.BitsPerPixel = 32;

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
LoadFileFromHardDrive(IN EFI_STRING FilePath, OUT EFI_PHYSICAL_ADDRESS *FileAddress, OUT UINT64 *FileSize) {
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_PHYSICAL_ADDRESS KernelAddress = 0;
  UINT64 KernelSize = 0;

  EFI_FILE_PROTOCOL *RootFile = NULL;

  Status = sSimpleFileSystemProtocol->OpenVolume(sSimpleFileSystemProtocol, &RootFile);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_PROTOCOL *KernelFile = NULL;

  Status = RootFile->Open(
    RootFile,
    &KernelFile,
    FilePath,
    EFI_FILE_MODE_READ,
    0);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  EFI_FILE_INFO *FileInfo = NULL;
  UINT64 InfoSize = SIZE_OF_EFI_FILE_INFO + 0xFF;

  Status = gBS->AllocatePool(
    EfiLoaderData,
    InfoSize,
    (VOID **)&FileInfo);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = KernelFile->GetInfo(
    KernelFile,
    &gEfiFileInfoGuid,
    &InfoSize,
    FileInfo);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  KernelSize = FileInfo->FileSize;

  UINT64 KernelPages = EFI_SIZE_TO_PAGES(KernelSize);

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

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
LoadKernelIntoMemory(VOID) {
  EFI_STATUS Status = EFI_SUCCESS;

  return Status;
}

STATIC
EFI_STATUS
EFIAPI
RetrieveMemoryMapAndExitBootServices(IN EFI_HANDLE ImageHandle) {
  EFI_STATUS Status = EFI_SUCCESS;

  EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
  UINT64 MemoryMapSize = 0;
  UINT64 MapKey = 0;
  UINT64 DescriptorSize = 0;
  UINT32 DescriptorVersion = 0;

  Status = gBS->GetMemoryMap(
    &MemoryMapSize,
    MemoryMap,
    &MapKey,
    &DescriptorSize,
    &DescriptorVersion);

  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  Status = gBS->AllocatePool(
    EfiLoaderData,
    MemoryMapSize,
    (VOID **)&MemoryMap);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->GetMemoryMap(
    &MemoryMapSize,
    MemoryMap,
    &MapKey,
    &DescriptorSize,
    &DescriptorVersion);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->ExitBootServices(ImageHandle, MapKey);

  sBootInfo.MemoryMap = MemoryMap;
  sBootInfo.MemoryMapSize = MemoryMapSize;
  sBootInfo.DescriptorSize = DescriptorSize;

  return Status;
}

EFI_STATUS
EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status = EFI_SUCCESS;

  // Disable the watchdog timer to prevent the system from resetting while we're loading the kernel and doing other work.

  Status = gBS->SetWatchdogTimer(0, 0, 0, NULL);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Retrieve the protocols that we will need to load the kernel and get information about the system.

  Status = RetrieveRequiredProtocols(ImageHandle);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Set the highest available graphics mode to ensure that the kernel has a proper frame buffer to work with.

  Status = SetHighestAvailableGraphicsMode();

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Retrieve information about the frame buffer so that we can pass it to the kernel.

  Status = RetrieveFrameBufferInformation();

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Load the kernel into memory.

  Status = LoadKernelIntoMemory();

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Print the addresses and sizes of the loaded kernel and the loader itself for debugging purposes.

  Print(L"Loader loaded at address: 0x%lx, size: %lu bytes\n", sLoadedImageProtocol->ImageBase, sLoadedImageProtocol->ImageSize);
  Print(L"Kernel loaded at address: 0x%lx, size: %lu bytes\n", sBootInfo.KernelAddress, sBootInfo.KernelSize);

  // Retrieve the system's memory map and pass it to the kernel. The kernel will need this information to manage memory.

  Status = RetrieveMemoryMapAndExitBootServices(ImageHandle);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Write the loaders's address to a known memory location so that GDB can find it. (This is a temporary solution)

  *(UINT64 *)0x10010 = (UINT64)sLoadedImageProtocol->ImageBase;
  *(UINT64 *)0x10008 = sBootInfo.KernelAddress;
  *(UINT64 *)0x10000 = 0xDEADBEEF;

  // Jump to the kernel's entry point.

  KERNEL_START Kernelstart = (KERNEL_START)sBootInfo.KernelAddress;

  Kernelstart(&sBootInfo);

  return Status;
}