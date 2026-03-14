MOUNT_POINT = DriveMount

LoaderNoopt:
	edk2/BaseTools/BinWrappers/PosixLike/build \
		-v \
		-a X64 \
		-t GCC5 \
		-b NOOPT \
		-p ../LoaderPkg/LoaderPkg.dsc
	cp edk2/Build/Loader/NOOPT_GCC5/X64/Loader.efi Loader.efi
	cp edk2/Build/Loader/NOOPT_GCC5/X64/Loader.debug Loader.debug

LoaderDebug:
	edk2/BaseTools/BinWrappers/PosixLike/build \
		-v \
		-a X64 \
		-t GCC5 \
		-b DEBUG \
		-p ../LoaderPkg/LoaderPkg.dsc
	cp edk2/Build/Loader/DEBUG_GCC5/X64/Loader.efi Loader.efi
	cp edk2/Build/Loader/DEBUG_GCC5/X64/Loader.debug Loader.debug

LoaderRelease:
	edk2/BaseTools/BinWrappers/PosixLike/build \
		-v \
		-a X64 \
		-t GCC5 \
		-b RELEASE \
		-p ../LoaderPkg/LoaderPkg.dsc
	cp edk2/Build/Loader/RELEASE_GCC5/X64/Loader.efi Loader.efi
	cp edk2/Build/Loader/RELEASE_GCC5/X64/Loader.debug Loader.debug

KernelDebug:
	cd Kernel && make Clean && make Kernel.bin

HardDrive:
	qemu-img create -f raw DRIVE.IMG 1G
	mkfs.fat -F32 DRIVE.IMG
	mkdir $(MOUNT_POINT)
	mount DRIVE.IMG $(MOUNT_POINT)
	mkdir -p $(MOUNT_POINT)/EFI/BOOT
	cp Loader.efi $(MOUNT_POINT)/EFI/BOOT/BOOTX64.EFI
	cp Kernel.bin $(MOUNT_POINT)/KERNEL.BIN
	umount $(MOUNT_POINT)
	rm -rf $(MOUNT_POINT)

BootEmu:
	qemu-system-x86_64 \
		-bios OVMF.FD \
		-drive file=DRIVE.IMG,format=raw,if=virtio \
		-m 1G \
		-vga std \
		-gdb tcp:0.0.0.0:9001 \
		-S &

GdbDebug:
	gdb -q -x .gdbinit Loader.debug

.PHONY: LoaderNoopt LoaderDebug LoaderRelease KernelDebug HardDrive BootEmu GdbDebug