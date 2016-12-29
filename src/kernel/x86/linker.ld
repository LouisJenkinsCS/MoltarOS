/*
	Bootloader will search for _start in boot.s... 
*/
ENTRY(start)

/*
	Where will the .o files be put?
*/
SECTIONS
{
	/*
		Start at address 3GB + 1MB (higher half kernel). Note: This means we need
		to properly offset all sections below.
	*/
	. = 0xC0100000;

	/*
		Place multiboot header to ensure the bootloader recognizes this file.
		Then place the text section.
	*/
	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
		*(.rodata*)
	}

	/*
		Then we place the Read-Write data
	*/
	.data ALIGN(0x1000) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	/*
		Uninitialized Read-Write data and the stack (.bss section)
	*/
	.bss : AT(ADDR(.bss) - 0xC0000000)
	{
		_sbss = .;
		*(COMMON)
		*(.bss)
		_ebss = .;
	}
}