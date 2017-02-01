global read_eip
	read_eip:
		; EAX is used as the return value for _cdecl
		; The only value on the stack at this moment is
		; the return address (instruction pointer)
		pop eax
		; Simulate a return (no ret required)
		jmp eax

; Here we:
; * Stop interrupts so we don't get interrupted.
; * Temporarily put the new EIP location in ECX.
; * Temporarily put the new page directory's physical address in EAX.
; * Set the base and stack pointers
; * Set the page directory
; * Put a dummy value (0x12345) in EAX so that above we can recognize that we've just
;   switched task.
; * Restart interrupts. The STI instruction has a delay - it doesn't take effect until after
;   the next instruction.
; * Jump to the location in ECX (remember we put the new EIP in there).
[GLOBAL perform_task_switch]
perform_task_switch:
     cli;
     mov ecx, [esp+4]   ; EIP
     mov ebp, [esp+8]  ; EBP
     mov esp, [esp+12]  ; ESP
     mov eax, 0x12345   ; magic number to detect a task switch
     sti;
     jmp ecx