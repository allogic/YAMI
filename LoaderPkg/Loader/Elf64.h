#ifndef ELF64_H
#define ELF64_H

#include <Uefi.h>

EFI_STATUS
EFIAPI
LoadElf64IntoMemory(IN EFI_STRING FileName);

#endif