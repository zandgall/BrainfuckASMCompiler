section .data
	memory dq 0

section .text
move_right_once:
	; lea pointer, [pointer+1]
	inc rbx
	ret

move_right:
	; lea pointer, [pointer+rdi]
	add rbx, rdi
	ret

move_left_once:
	; lea pointer, [pointer-1]
	dec rbx
	ret

move_left:
	; lea pointer, [pointer-rdi]
	sub rbx, rdi
	ret

increment_once:
	; mov rsi, qword[pointer]
	inc byte [rbx]
	ret

increment:
	; mov rsi, qword[pointer]
	; add byte [rsi], dil
	add byte [rbx], dil
	ret

decrement_once:
	; mov rsi, qword[pointer]
	; dec byte [rsi]
	dec byte [rbx]
	ret

decrement:
	; mov rsi, qword[pointer]
	; sub byte [rsi], dil
	sub byte [rbx], dil
	ret

output:
	mov rax, 1
	mov rdx, 1
	mov rsi, rbx
	mov rdi, 1
	syscall
	ret

input:
	mov rax, 0
	mov rdx, 1
	mov rsi, rbx
	mov rdi, 0
	syscall
	ret
