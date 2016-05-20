; idt_handler.asm

; This macro is used to generate an interrupt service request which do not produce errors.
%macro isr_generate_noerr 1
	; Definite preprocessed global label
	global interrupt_service_request_%1

		interrupt_service_request_%1:
			; Disable interrupts
			cli

			; Push null error code
			push 0
			; Push interrupt number
			push %1

			; Jump into our interrupt setup handler
			jmp idt_setup
%endmacro

; This macro is used to generate an interrupt service request handler which do produce errors.
; TODO: Please fix this up, violating the YAGNI rule over and over and over again.
%macro isr_generate_err 1
	; Definite preprocessed global label
	global interrupt_service_request_%1

		interrupt_service_request_%1:
			; Disable interrupts
			cli

			; Push interrupt number
			push %1

			; Jump into our interrupt setup handler
			jmp idt_setup
%endmacro

; Remap an IRQ number and generate an IRQ handler.
; %1 - Original IRQ number.
; %2 - Desired IRQ number to remap to.
%macro irq_generate 2
	global interrupt_request_%1

		interrupt_request_%1:
			; Disable interrupts
			cli

			; No error code
			push byte 0
			; Set int_num to the remapped value
			push byte %2

			; Jump into our interrupt setup handler
			jmp irq_setup
%endmacro

isr_generate_noerr 0
isr_generate_noerr 1
isr_generate_noerr 2
isr_generate_noerr 3
isr_generate_noerr 4
isr_generate_noerr 5
isr_generate_noerr 6
isr_generate_noerr 7
isr_generate_err   8
isr_generate_noerr 9
isr_generate_err   10
isr_generate_err   11
isr_generate_err   12
isr_generate_err   13
isr_generate_err   14
isr_generate_noerr 15
isr_generate_noerr 16
isr_generate_noerr 17
isr_generate_noerr 18
isr_generate_noerr 19
isr_generate_noerr 20
isr_generate_noerr 21
isr_generate_noerr 22
isr_generate_noerr 23
isr_generate_noerr 24
isr_generate_noerr 25
isr_generate_noerr 26
isr_generate_noerr 27
isr_generate_noerr 28
isr_generate_noerr 29
isr_generate_noerr 30
isr_generate_noerr 31
isr_generate_noerr 255


global idt_setup

idt_setup:
	; Push all registers onto the stack
	pusha

	; As the data segment register is a 16-bit register, we simply store it in AX, the
	; lower 16-bits of the EAX register. This is needed because the struct registers
	; requires word-aligned data (4-bytes).
	mov ax, ds
	push eax

	; Load kernel data segment descriptor (0x10)
	mov ax, 0x10
	mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Push stack pointer, which will be treated as (struct registers *) in the function call
    push esp

    ; Call C-function idt_handler with the registers as parameter
    extern idt_handler
    call idt_handler

    ; Cleanup (struct registers *) parameter
    add esp, 4

    ; Restore data segment register (What was pushed from eax). Since again, the segment
    ; registers are 16-bit registers, we must use the first 16-bit register, bx, and move them
    ; back into the segment registered.
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    ; Restore registers pushed during pusha instruction
    popa
    ; Cleanup error code and interrupt number
    add esp, 8
    ; Handles returning from interrupts
    iret

irq_generate 0, 32
irq_generate 1, 33
irq_generate 2, 34
irq_generate 3, 35
irq_generate 4, 36
irq_generate 5, 37
irq_generate 6, 38
irq_generate 7, 39
irq_generate 8, 40
irq_generate 9, 41
irq_generate 10, 42
irq_generate 11, 43
irq_generate 12, 44
irq_generate 13, 45
irq_generate 14, 46
irq_generate 15, 47

; TODO: This has only a small variation compared to the above (only variation being that it called irq_handler instead)
global irq_setup

irq_setup:
	; Push all registers onto the stack
	pusha

	; As the data segment register is a 16-bit register, we simply store it in AX, the
	; lower 16-bits of the EAX register. This is needed because the struct registers
	; requires word-aligned data (4-bytes).
	mov ax, ds
	push eax

	; Load kernel data segment descriptor (0x10)
	mov ax, 0x10
	mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Push stack pointer, which will be treated as (struct registers *) in the function call
    push esp

    ; Call C-function idt_handler with the registers as parameter
    extern irq_handler
    call irq_handler

    ; Cleanup (struct registers *) parameter
    add esp, 4

    ; Restore data segment register (What was pushed from eax). Since again, the segment
    ; registers are 16-bit registers, we must use the first 16-bit register, bx, and move them
    ; back into the segment registered.
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    mov ss, bx

    ; Restore registers pushed during pusha instruction
    popa
    ; Cleanup error code and interrupt number
    add esp, 8
    ; Handles returning from interrupts
    iret