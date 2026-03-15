#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "Types.h"

typedef struct FRAME_BUFFER {
  PHYSICAL_ADDRESS Buffer;
  UINT64 BufferSize;
  UINT32 Width;
  UINT32 Height;
  UINT32 PixelsPerScanLine;
  UINT32 BitsPerPixel;
} FRAME_BUFFER;

EXTERN FRAME_BUFFER *gFrameBuffer;

VOID ClearScreen(UINT32 Color);

VOID DrawPixel(UINT32 PositionX, UINT32 PositionY, UINT32 Color);
VOID DrawRect(UINT32 PositionX, UINT32 PositionY, UINT32 Width, UINT32 Height, UINT32 Color);

#endif