; gdt_flush -- Will apply the passed gdt_ptr to the CPU.

%define NULL_DESC 0x0
%define CODE_DESC 0x8
%define DATA_DESC 0x10

global gdt_flush

gdt_flush:
	; Obtain the gdt_ptr passed and load it
	mov eax, [esp + 4]
	lgdt [eax]

	; All data segment selectors point to the data segment descriptor
	mov ax, DATA_DESC
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Now, we do a far jump to flush label in code segment descriptor (Why?)
	jmp CODE_DESC:.flush
.flush:
	ret
