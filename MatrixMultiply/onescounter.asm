%include "asm_io.inc"
extern printf,scanf


segment .data
msg db 'got you %lf',0
format db '%lf',0
x dq 123

segment .bss
n resq 1
segment .text
global asm_main

asm_main:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    sub rsp, 8

    ; -------------------------
    mov rdi , format
    mov rsi , n
    call scanf
    movsd xmm0,[n]
    mov rdi , format
    mov rsi , n
    call scanf
    movsd xmm1,[n]
    addps xmm0,xmm1
    mov rdi,msg
    call printf
    
    
    ; -------------------------
  
    
    
_end:
	add rsp, 8
	pop r15
	pop r14
	pop r13
	pop r12
    pop rbx
    pop rbp

	ret
	
	
	
	
  
