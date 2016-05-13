/*
	Bootloader will search for _start in boot.s... 
*/
ENTRY(_start)

/*
	Where will the .o files be put?
*/
SECTIONS
{
	/*
		Start at address 1MB
	*/
	. = 1M;

	/*
		Place multiboot header to ensure the bootloader recognizes this file.
		Then place the text section.
	*/
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	/*
		Now we place the Read-Only data
	*/
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	/*
		Then we place the Read-Write data
	*/
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	/*
		Uninitialized Read-Write data and the stack (.bss section)
	*/
	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
		*(.create_stack)
	}

	/*
		Any compiler-generated sections will be, by default, placed below here.
	*/
}