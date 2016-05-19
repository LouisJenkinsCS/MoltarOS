; idt_flush -- Setup and initialize the interrupt descriptor table.

global idt_flush

idt_flush:
	; Obtain idt_ptr structure on stack and load it.
	mov eax, [esp + 4]
	lidt [eax]
	ret