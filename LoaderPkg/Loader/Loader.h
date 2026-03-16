#ifndef LOADER_H
#define LOADER_H

#include <Uefi.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>

extern EFI_GRAPHICS_OUTPUT_PROTOCOL *gGraphicsOutputProtocol;
extern EFI_LOADED_IMAGE_PROTOCOL *gLoadedImageProtocol;
extern EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *gSimpleFileSystemProtocol;

#endif