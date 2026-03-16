ENTRY(KernelStart)

SECTIONS
{
	. = 0x100000;

	__kernel_start__ = .;

	.text :
	{
		__text_start__ = .;
		*(.text.KernelStart)
		*(.text*)
		__text_end__ = .;
	}

	. = ALIGN(0x1000);

	.rodata :
	{
		__rodata_start__ = .;
		*(.rodata*)
		__rodata_end__ = .;
	}

	__rodata_init__ = LOADADDR(.rodata);

	. = ALIGN(0x1000);

	.data :
	{
		__data_start__ = .;
		*(.data*)
		__data_end__ = .;
	}

	__data_init__ = LOADADDR(.data);

	. = ALIGN(0x1000);

	.bss (NOLOAD) :
	{
		__bss_start__ = .;
		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
	}

	. = ALIGN(0x1000);

	__kernel_end__ = .;
}