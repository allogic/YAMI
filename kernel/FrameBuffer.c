#include "FrameBuffer.h"

FRAME_BUFFER *gFrameBuffer = 0;

VOID ClearScreen(UINT32 Color) {
  for (UINT32 X = 0; X < gFrameBuffer->Width; X++) {
    for (UINT32 Y = 0; Y < gFrameBuffer->Height; Y++) {
      DrawPixel(X, Y, Color);
    }
  }
}

VOID DrawPixel(UINT32 PositionX, UINT32 PositionY, UINT32 Color) {
  UINT32 *Pixel = (UINT32 *)gFrameBuffer->Buffer + PositionY * gFrameBuffer->PixelsPerScanLine + PositionX;
  *Pixel = Color;
}
VOID DrawRect(UINT32 PositionX, UINT32 PositionY, UINT32 Width, UINT32 Height, UINT32 Color) {
  for (UINT32 X = PositionX; X < (PositionX + Width); X++) {
    for (UINT32 Y = PositionY; Y < (PositionX + Height); Y++) {
      DrawPixel(X, Y, Color);
    }
  }
}