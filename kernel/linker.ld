ENTRY(KernelMain)

SECTIONS
{
	__kernel_start__ = .;

	.text :
	{
		. = ALIGN(4096);
		__text_start__ = .;
		*(.text*)
		. = ALIGN(4096);
		__text_end__ = .;
	}

	.rodata :
	{
		. = ALIGN(4096);
		__rodata_start__ = .;
		*(.rodata*)
		. = ALIGN(4096);
		__rodata_end__ = .;
	}

	.data :
	{
		. = ALIGN(4096);
		__data_start__ = .;
		*(.data*)
		. = ALIGN(4096);
		__data_end__ = .;
	}

	.bss (NOLOAD) :
	{
		. = ALIGN(4096);
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4096);
		__bss_end__ = .;
	}

	__kernel_end__ = .;
}