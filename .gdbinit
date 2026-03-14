target extended-remote localhost:9001

set listsize 30
set print pretty on
set print array on

watch *(unsigned long long)0x10000 == 0xDEADBEEF

continue

set $base = *(unsigned long long)0x10008

add-symbol-file Loader.debug -o $base