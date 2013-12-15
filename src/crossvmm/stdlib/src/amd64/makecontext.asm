[GLOBAL setmcontext]
setmcontext:
	mov	rsi, [ 16 + rdi]
	mov	rdx, [ 24 + rdi]
	mov	rcx, [ 32 + rdi]
	mov	r8,  [ 40 + rdi]
	mov	r9,  [ 48 + rdi]
	mov	rax, [ 56 + rdi]
	mov	rbx, [ 64 + rdi]
	mov	rbp, [ 72 + rdi]
	mov	r10, [ 80 + rdi]
	mov	r11, [ 88 + rdi]
	mov	r12, [ 96 + rdi]
	mov	r13, [104 + rdi]
	mov	r14, [112 + rdi]
	mov	r15, [120 + rdi]
	mov	rsp, [184 + rdi]
	push	qword [160 + rdi]         ; new rip
	mov	rdi, [8 + rdi]
	ret

[GLOBAL getmcontext]
getmcontext:
	mov	[  8 + rdi], rdi
	mov	[ 16 + rdi], rsi
	mov	[ 24 + rdi], rdx
	mov	[ 32 + rdi], rcx
	mov	[ 40 + rdi], r8
	mov	[ 48 + rdi], r9
	mov	qword [ 56 + rdi], 1
	mov	[ 64 + rdi], rbx
	mov	[ 72 + rdi], rbp
	mov	[ 80 + rdi], r10
	mov	[ 88 + rdi], r11
	mov	[ 96 + rdi], r12
	mov	[104 + rdi], r13
	mov	[112 + rdi], r14
	mov	[120 + rdi], r15

	mov	rcx, [rsp]          ; rip
	mov	[160 + rdi], rcx
	lea	rcx, [8 + rsp]      ; rsp
	mov	[184 + rdi], rcx

	mov	rcx, [32 + rdi]     ; restore rcx
	mov	rax, 0
	ret

