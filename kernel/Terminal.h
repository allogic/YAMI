#ifndef TERMINAL_H
#define TERMINAL_H

#include "Types.h"

VOID DrawChar(CHAR Char, UINT32 PositionX, UINT32 PositionY, UINT32 Color);
VOID DrawString(STRING String, UINT32 PositionX, UINT32 PositionY, UINT32 Color);

#endif