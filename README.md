# ALTOS
x64 toy kernel written in C.

### Prerequisites
```
sudo apt update
sudo apt install build-essential uuid-dev iasl nasm python3 dosfstools git

sudo ln -s /usr/bin/python3 /usr/bin/python
```

### Setup EDK2
Clone latest stable release
```
git clone https://github.com/tianocore/edk2

cd edk2

git submodule update --init
git checkout tags/edk2-stable202602

source edksetup.sh
```
Build the BaseTools (https://www.tianocore.org/tianocore-wiki/build-tooling/build-workflows/base_tools.html)
```
cd edk2/BaseTools && make
```
Build BIOS for QEMU (https://www.tianocore.org/tianocore-wiki/development/tutorials-howto/how_to_build_ovmf.html)
```
cd edk2/OvmfPkg && ./build.sh
```