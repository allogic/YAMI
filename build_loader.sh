#!/bin/bash

build_cmd="edk2/BaseTools/BinWrappers/PosixLike/build"

"$build_cmd" -a X64 -t GCC5 -p ../LoaderPkg/LoaderPkg.dsc -v

cp edk2/Build/Loader/DEBUG_GCC5/X64/LoaderPkg/Loader/Loader/OUTPUT/Loader.efi loader.efi