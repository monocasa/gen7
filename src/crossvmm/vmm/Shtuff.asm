[BITS 64]
[GLOBAL GetCR0]
GetCR0:
	mov rax, cr0
	ret

[GLOBAL GetCR2]
GetCR2:
	mov rax, cr2
	ret

[GLOBAL GetCR3]
GetCR3:
	mov rax, cr3
	ret

[GLOBAL GetCR4]
GetCR4:
	mov rax, cr4
	ret

