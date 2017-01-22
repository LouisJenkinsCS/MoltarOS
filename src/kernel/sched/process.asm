global read_eip
	read_eip:
		; EAX is used as the return value for _cdecl
		; The only value on the stack at this moment is
		; the return address (instruction pointer)
		pop eax
		; Simulate a return (no ret required)
		jmp eax