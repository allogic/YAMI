#include <Library/UefiBootServicesTableLib.h>

#include "Elf64.h"
#include "Loader.h"

typedef struct ELF64_HEADER {
  UINT8 Ident[16];  // File identifier
  UINT64 Reserved;  // Reserved
  UINT16 Type;      // File type
  UINT16 Machine;   // Machine architecture
  UINT32 Version;   // ELF format version
  UINT64 Entry;     // Entry point
  UINT64 Phoff;     // Program header file offset
  UINT64 Shoff;     // Section header file offset
  UINT32 Flags;     // Architecture-specific flags
  UINT16 Ehsize;    // Size of ELF header in bytes
  UINT16 Phentsize; // Size of program header entry
  UINT16 Phnum;     // Number of program header entries
  UINT16 Shentsize; // Size of section header entry
  UINT16 Shnum;     // Number of section header entries
  UINT16 Shstrndx;  // Section name strings section
} ELF64_HEADER;

typedef struct ELF64_PROGRAM_HEADER {
  UINT32 Type;   // Segment type
  UINT32 Flags;  // Segment attributes
  UINT64 Offset; // Offset in file
  UINT64 Vaddr;  // Virtual address in memory
  UINT64 Paddr;  // Physical address in memory
  UINT64 Filesz; // Size of segment in file
  UINT64 Memsz;  // Size of segment in memory
  UINT64 Align;  // Segment alignment
} ELF64_PROGRAM_HEADER;

typedef struct ELF64_SECTION_HEADER {
  UINT32 Name;      // Section name index in the section header string table
  UINT32 Type;      // Section type
  UINT64 Flags;     // Section flags
  UINT64 Addr;      // Section virtual addr at execution
  UINT64 Offset;    // Section file offset
  UINT64 Size;      // Section size in bytes
  UINT32 Link;      // Link to another section
  UINT32 Info;      // Additional section information
  UINT64 Addralign; // Section alignment
  UINT64 Entsize;   // Entry size if section holds table
} ELF64_SECTION_HEADER;

EFI_STATUS
EFIAPI
LoadElf64IntoMemory(IN EFI_STRING FileName) {
  EFI_STATUS Status = EFI_SUCCESS;

  // Open the root directory of the file system to access the ELF file.

  EFI_FILE_PROTOCOL *RootFile = NULL;

  Status = gSimpleFileSystemProtocol->OpenVolume(
    gSimpleFileSystemProtocol,
    &RootFile);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Open the ELF file from the root directory.

  EFI_FILE_PROTOCOL *ElfFile = NULL;

  Status = RootFile->Open(
    RootFile,
    &ElfFile,
    FileName,
    EFI_FILE_MODE_READ,
    0);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Read the header from the ELF file to get information about the program headers and entry point.

  ELF64_HEADER ElfHeader = {0};
  UINT64 ElfHeaderSize = sizeof(ELF64_HEADER);

  Status = ElfFile->Read(
    ElfFile,
    &ElfHeaderSize,
    &ElfHeader);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Verify that the file is a valid ELF file by checking the magic number in the header.

  if (ElfHeader.Ident[0] != 0x7F ||
      ElfHeader.Ident[1] != 'E' ||
      ElfHeader.Ident[2] != 'L' ||
      ElfHeader.Ident[3] != 'F') {
    return EFI_UNSUPPORTED;
  }

  // Set the file position to the start of the program headers.

  Status = ElfFile->SetPosition(ElfFile, ElfHeader.Phoff);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Read the program headers from the ELF file to get information about the segments that need to be loaded into memory.

  ELF64_PROGRAM_HEADER *ElfProgramHeader = NULL;
  UINT64 ProgramHeaderCount = ElfHeader.Phnum;
  UINT64 ProgramHeaderSize = sizeof(ELF64_PROGRAM_HEADER) * ProgramHeaderCount;

  Status = gBS->AllocatePool(
    EfiLoaderData,
    ProgramHeaderSize,
    &ElfProgramHeader);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = ElfFile->Read(
    ElfFile,
    &ProgramHeaderSize,
    ElfProgramHeader);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Iterate through the program headers and load the segments described by them into memory.

  for (UINT64 ProgramHeaderIndex = 0; ProgramHeaderIndex < ProgramHeaderCount; ProgramHeaderIndex++) {

    ELF64_PROGRAM_HEADER *ProgramHeader = &ElfProgramHeader[ProgramHeaderIndex];

    // We are only interested in program headers of type 1, which describe loadable segments that need to be loaded into memory.

    if (ProgramHeader->Type != 1) {
      continue;
    }

    // Allocate memory for the segment described by the program header.

    EFI_PHYSICAL_ADDRESS SegmentAddress = ProgramHeader->Paddr;
    UINT64 SegmentSize = ProgramHeader->Filesz;

    Status = gBS->AllocatePages(
      AllocateAddress,
      EfiLoaderData,
      EFI_SIZE_TO_PAGES(SegmentSize),
      (VOID *)SegmentAddress);

    if (EFI_ERROR(Status)) {
      return Status;
    }

    // Set the file position to the start of the segment in the ELF file.

    Status = ElfFile->SetPosition(ElfFile, ProgramHeader->Offset);

    if (EFI_ERROR(Status)) {
      return Status;
    }

    // Read the segment from the ELF file into memory.

    Status = ElfFile->Read(
      ElfFile,
      SegmentSize,
      (VOID *)SegmentAddress);

    if (EFI_ERROR(Status)) {
      return Status;
    }

    // If the segment's size in memory is larger than its size in the file, we need to zero out the remaining bytes in memory.

    gBS->SetMem(
      (VOID *)(SegmentAddress + ProgramHeader->Filesz),
      ProgramHeader->Memsz - ProgramHeader->Filesz,
      0);
  }

  // Close the ELF file as we have read all the necessary information from it.

  Status = RootFile->Close(ElfFile);

  return Status;
}