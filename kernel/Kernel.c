#include "Types.h"
#include "Terminal.h"
#include "FrameBuffer.h"

typedef struct MEMORY_DESCRIPTOR {
  UINT32 Type;
  PHYSICAL_ADDRESS PhysicalStart;
  VIRTUAL_ADDRESS VirtualStart;
  UINT64 NumberOfPages;
  UINT64 Attributes;
} MEMORY_DESCRIPTOR;

typedef struct BOOT_INFO {
  PHYSICAL_ADDRESS KernelAddress;
  UINT64 KernelSize;
  FRAME_BUFFER FrameBuffer;
  MEMORY_DESCRIPTOR *MemoryMap;
  UINT64 MemoryMapSize;
  UINT64 DescriptorSize;
} BOOT_INFO;

VOID KernelMain(BOOT_INFO *BootInfo) {
  gFrameBuffer = &BootInfo->FrameBuffer;

  ClearScreen(0x00000000);
  DrawString("Hello, World!", 100, 100, 0xFFFFFFFF);
  DrawChar('H', 100, 200, 0xFFFFFFFF);
  DrawRect(100, 100, 100, 100, 0xFFFFFFFF);

  while (1)
    ;
}