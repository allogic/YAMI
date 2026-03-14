[Defines]
	PLATFORM_NAME           = Loader
	PLATFORM_GUID           = 3328A84E-037E-4782-9B3B-CC4087CE01A4
	PLATFORM_VERSION        = 1.0
	DSC_SPECIFICATION       = 0x00010005
	OUTPUT_DIRECTORY        = Build/Loader
	SUPPORTED_ARCHITECTURES = X64
	BUILD_TARGETS           = DEBUG|RELEASE|NOOPT
	SKUID_IDENTIFIER        = DEFAULT

[LibraryClasses]
	UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
	UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
	UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
	PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
	PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
	DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
	BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
	BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
	FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
	MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
	RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
	DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
	UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
	StackCheckLib|MdePkg/Library/StackCheckLibNull/StackCheckLibNull.inf

[Components]
	../LoaderPkg/Loader/Loader.inf