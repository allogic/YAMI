target extended-remote localhost:9001

set disassembly-flavor intel
set listsize 30
set print pretty on
set print array on

tui enable
layout split

watch *(unsigned long long)0x10000 == 0xDEADBEEF

continue

set $loader_base = *(unsigned long long)0x10010
set $kernel_base = *(unsigned long long)0x10008

add-symbol-file Loader.debug -o $loader_base
add-symbol-file Kernel/Kernel.elf -o $kernel_base