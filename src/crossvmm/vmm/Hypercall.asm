[BITS 64]
[GLOBAL hypercall_0]
[GLOBAL hypercall_1]
hypercall_0:
hypercall_1:
	out 0, eax
	ret

