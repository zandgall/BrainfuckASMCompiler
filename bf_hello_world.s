%include "bfstd.s"

section .text
global _start
_start:
	
	; Allocate 30,000 cells
	mov rax, 9								; Make a system call to mmap
	mov rsi, 30000							; Move the size to rsi
	mov rdi, 0								; Let the system decide the direction
	mov rdx, 3								; Read / writable
	mov r8, -1								; To memory
	mov r9, 0								; With no offset
	mov r10, 0x22							; Private and anonymous
	syscall

	mov qword[memory], rax
	mov rbx, qword[memory] ; Point to beginning of memory

	mov rdi, 8
	call increment
	l0:
		call move_right_once
		mov rdi, 4
		call increment
		l1:
			call move_right_once
			mov rdi, 2
			call increment
			call move_right_once
			mov rdi, 3
			call increment
			call move_right_once
			mov rdi, 3
			call increment
			call move_right_once
			call increment_once
			mov rdi, 4
			call move_left
			call decrement_once
			cmp byte [rbx], 0
			jne l1
		call move_right_once
		call increment_once
		call move_right_once
		call increment_once
		call move_right_once
		call decrement_once
		mov rdi, 2
		call move_right
		call increment_once
		l2:
			call move_left_once
			cmp byte [rbx], 0
			jne l2
		call move_left_once
		call decrement_once
		cmp byte [rbx], 0
		jne l0
	mov rdi, 2
	call move_right
	call output
	call move_right_once
	mov rdi, 3
	call decrement
	call output
	mov rdi, 7
	call increment
	call output
	call output
	mov rdi, 3
	call increment
	call output
	mov rdi, 2
	call move_right
	call output
	call move_left_once
	call decrement_once
	call output
	call move_left_once
	call output
	mov rdi, 3
	call increment
	call output
	mov rdi, 6
	call decrement
	call output
	mov rdi, 8
	call decrement
	call output
	mov rdi, 2
	call move_right
	call increment_once
	call output
	call move_right
	mov rdi, 2
	call increment
	call output

	mov rax, 60
	mov rdi, 0
	syscall