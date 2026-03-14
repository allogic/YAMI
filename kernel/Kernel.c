#define VOID void

typedef char INT8;
typedef short INT16;
typedef int INT32;
typedef long long INT64;

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;

typedef float REAL32;
typedef double REAL64;

typedef struct FRAME_BUFFER_INFO {
  VOID *FrameBuffer;
  UINT64 FrameBufferSize;
  UINT32 Width;
  UINT32 Height;
  UINT32 PixelsPerScanLine;
  UINT32 BitsPerPixel;
} FRAME_BUFFER_INFO;

VOID SetPixel(FRAME_BUFFER_INFO *FrameBufferInfo, UINT32 X, UINT32 Y, UINT32 Color) {
  UINT32 *Pixel = (UINT32 *)FrameBufferInfo->FrameBuffer + Y * FrameBufferInfo->PixelsPerScanLine + X;
  *Pixel = Color;
}

VOID KernelMain(FRAME_BUFFER_INFO *FrameBufferInfo) {
  while (1) {
    for (UINT32 X = 0; X < FrameBufferInfo->Width; X++) {
      for (UINT32 Y = 0; Y < FrameBufferInfo->Height; Y++) {
        SetPixel(FrameBufferInfo, X, Y, 0xFFFFFFFF);
      }
    }
  }
}