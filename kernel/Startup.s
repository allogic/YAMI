global KernelStart

extern __rodata_init__
extern __rodata_start__
extern __rodata_end__

extern __data_init__
extern __data_start__
extern __data_end__

extern __bss_start__
extern __bss_end__

extern KernelMain

section .text

KernelStart:

	; Ensure string ops go forward
	;cld
	push rcx

	; ============================================================
	; Copy .rodata section
	; ============================================================

	; Image source address
	lea rsi, [rel __rodata_init__]

	; Destination runtime address
	lea rdi, [rel __rodata_start__]

	; Size of section in bytes
	lea rcx, [rel __rodata_end__]
	sub rcx, rdi

	; Convert to qwords
	shr rcx, 3

	; Copy RCX * 8 bytes
	rep movsq

	; ============================================================
	; Copy .data section
	; ============================================================

	; Image source address
	lea rsi, [rel __data_init__]

	; Destination runtime address
	lea rdi, [rel __data_start__]

	; Size of section in bytes
	lea rcx, [rel __data_end__]
	sub rcx, rdi

	; Convert to qwords
	shr rcx, 3

	; Copy RCX * 8 bytes
	rep movsq

	; ============================================================
	; Clear .bss section
	; ============================================================

	; Destination runtime address
	lea rdi, [rel __bss_start__]

	; Size of section in bytes
	lea rcx, [rel __bss_end__]
	sub rcx, rdi

	; Convert to qwords
	shr rcx, 3

	; Value to write
	xor rax, rax

	; Fill with zeros
	rep stosq

	; ============================================================
	; Init stack
	; ============================================================

	; Initialize stack pointer to the top of the stack
	mov rsp, StackTop

	; Align it to a 16 byte boundary
	and rsp, -16

	; ============================================================
	; Enter C kernel
	; ============================================================

	pop rcx

	call KernelMain

.KernelHalt:

	hlt
	jmp .KernelHalt

section .bss

StackBottom:

	resb 16384

StackTop:

section .note.GNU-stack noalloc noexec nowrite progbits