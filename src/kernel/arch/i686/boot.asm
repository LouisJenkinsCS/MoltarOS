; Constant Declarations for Multiboot Header (GRUB)
ALIGN_ON_PAGE equ 1 << 0 ; Align modules on page boundaries
USE_MMAP equ 1 << 1 ; Provide memory map
FLAGS equ ALIGN_ON_PAGE | USE_MMAP ; Multiboot flag
MAGIC equ 0x1BADB002 ; 'Magic Number' needed to allow bootloader to find header
CHECKSUM equ -(MAGIC + FLAGS) ; Checksum to prove we are multiboot

; Declare the multiboot header needed by GRUB
section .multiboot

	align 4
		dd MAGIC
		dd FLAGS
		dd CHECKSUM

; Setup stack pointer register (esp), as its contents are currently undefined. Allocate 16Mbs
section .create_stack nobits

	align 4
		stack_bottom:
			resb 1024 * 16
		stack_top:

; Setting up the stack and finally passing control back to our preferred language (C)
section .text
global _start

	_start:
		; Setup the stack pointer to point to the stack allocated above
		mov esp, stack_top

		; Initialize the core facilities of the kernel
		extern kernel_init
		call kernel_init

		; Calls any global constructors '__attribute__((constructor))'
		extern _init
		call _init

		; Finally, jump to kmain, and leave assembly behind for good
		extern kernel_main
		call kernel_main

		; If we do end up back here, the bootloader is gone and hence there is nothing left to do but halt permanently
		cli
	.hang:
		hlt
		jmp .hang