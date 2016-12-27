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

section .text

; gdt_flush -- Load the GDT (Global Descriptor Table)
global gdt_flush

	gdt_flush:
		; Obtain the gdt_ptr passed and load it
		mov eax, [esp + 4]
		lgdt [eax]

		; Setup the kernel's Data Segment Descriptor
		mov ax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		mov ss, ax

		; Far Jump to the kernel's Code Segment flush label
		jmp 0x8:.flush
	.flush:
		ret

; idt_flush -- Load the IDT (Interrupt Descriptor Table)
global idt_flush

	idt_flush:
		; Obtain idt_ptr structure on stack and load it.
		mov eax, [esp + 4]
		lidt [eax]
		ret

; Setting up the stack and finally passing control back to our preferred language (C)
global _start

	_start:
		; Setup the stack pointer to point to the stack allocated above
		mov esp, stack_top

		; EBX contains a pointer to the multiboot info structure that we should save for later
		push ebx

		; Zero EBP which is used to signify the end of a stack trace
		mov ebp, 0

		; Initialize the core facilities of the kernel
		extern kernel_init
		call kernel_init

		; Clean up stack frame
		add esp, 4

		; Zero EBP again since kernel_init will have changed it
		mov ebp, 0

		; Finally, jump to kmain, and leave assembly behind for good
		extern kernel_main
		call kernel_main

		; If we do end up back here, the bootloader is gone and hence there is nothing left to do but halt permanently
		cli
	.hang:
		hlt
		jmp .hang