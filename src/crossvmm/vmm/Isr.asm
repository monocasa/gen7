[GLOBAL isr]
[BITS 64]
[EXTERN hypercall_0]

isr:
	push rax
	push rbx
	push rcx
	push rdx
	push rsi
	push rdi
	push rsp
	push rbp
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov  rdi, 1
	mov  rsi, 'B'
	call hypercall_0
	mov  rsi, 'K'
	call hypercall_0
	mov  rsi, 'P'
	call hypercall_0
	mov  rsi, 'T'
	call hypercall_0
	mov  rsi, 0x0a
	call hypercall_0

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rbp
	pop rsp
	pop rdi
	pop rsi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	iretq

