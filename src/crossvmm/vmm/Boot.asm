[BITS 32]
[SECTION .boot]
[GLOBAL EntryPoint]
[EXTERN Stack]
EntryPoint:
	mov	eax, Gdtr1
	lgdt	[eax]

	push	0x08
	push	.GdtReady
	retf

.GdtReady:
	mov	eax, 0x10
	mov	ds, ax
	mov	ss, ax
	mov	esp, Stack

	call	SetupPagingAndLongMode

	mov	eax, Gdtr2
	lgdt	[Gdtr2]

	push	0x08
	push	.Gdt2Ready
	retf

[BITS 64]
[EXTERN kmain]
[EXTERN hypercall_0]
.Gdt2Ready:
	mov	eax, 0x10
	mov	ds, ax
	mov	es, ax
	mov	ss, ax

	mov	rsp, Stack + 0xFFFFFFFF80000000

	mov	rax, Gdtr3
	lgdt	[rax]

	mov	rax, kmain
	call	rax

	mov rax, hypercall_0
	mov rdi, 0
	call rax

[BITS 32]
[EXTERN __boot_pml4]
[EXTERN __boot_pml3]
[EXTERN __boot_pml2]
SetupPagingAndLongMode:
	mov	eax, __boot_pml3
	or	eax, 1
	mov	[__boot_pml4], eax
	mov	[__boot_pml4 + 0xFF8], eax

	mov	eax, __boot_pml2
	or	eax, 1
	mov	[__boot_pml3], eax
	mov	[__boot_pml3 + 0xFF0], eax

	; Map bottom 8MiB using 2MiB pages
	mov	dword [__boot_pml2], 0x000083
	mov	dword [__boot_pml2 + 8], 0x200083
	mov	dword [__boot_pml2 + 16], 0x400083
	mov	dword [__boot_pml2 + 24], 0x600083

	; Enable SSE
	mov	eax, cr0
	and	ax, 0xFFFB
	or	ax, 0x2
	mov	cr0, eax

	mov	eax, cr4
	or	ax, 3 << 9
	mov	cr4, eax

	ldmxcsr	[InitialMXCSR]

	; Load CR3 with PML4
	mov	eax, __boot_pml4
	mov	cr3, eax

	; Enable PAE
	mov	eax, cr4
	or	eax, 1 << 5
	mov	cr4, eax

	; Enable Long Mode in the MSR
	mov	ecx, 0xC0000080
	rdmsr
	or	eax, 1 << 8
	wrmsr

	; Enable Paging
	mov	eax, cr0
	or	eax, 1 << 31
	mov	cr0, eax

	ret

TmpGdt:
	DQ	0x0000000000000000
	DQ	0x00CF9A000000FFFF
	DQ	0x00CF92000000FFFF
	DQ	0x0000000000000000
	DQ	0x00A09A0000000000
	DQ	0x00A0920000000000

Gdtr1:
	DW	23
	DD	TmpGdt

Gdtr2:
	DW	23
	DD	TmpGdt + 24
	DD	0

Gdtr3:
	DW	23
	DQ	TmpGdt + 24 + 0xFFFFFFFF80000000

InitialMXCSR:
	DW	0x00001F80

