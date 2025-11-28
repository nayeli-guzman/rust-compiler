.data
print_fmt: 
.string "%ld \n"
.text
.globl hola
hola:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq $10, %rax
 movq %rax, -8(%rbp)
 movq $20, %rax
 movq %rax, -16(%rbp)
 movq $7, %rax
 movq %rax, 0(%rdi)
 movq $9, %rax
 movq %rax, 8(%rdi)
 movq %rdi, %rax
 jmp .end_hola
.end_hola:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 leaq -16(%rbp), %rcx
 subq $16, %rsp
 movq %rsp, %rdi
 call hola
 movq %rax, %rsi
 movq %rcx, %rdi
 movq $2, %rcx
 rep movsq
 movq %rcx, %rax
 leaq -16(%rbp), %rax
 movq %rax, %rcx
 movq $0, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 leaq -16(%rbp), %rax
 movq %rax, %rcx
 movq $1, %rax
 imulq $8, %rax
 addq %rax, %rcx
 movq (%rcx), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
